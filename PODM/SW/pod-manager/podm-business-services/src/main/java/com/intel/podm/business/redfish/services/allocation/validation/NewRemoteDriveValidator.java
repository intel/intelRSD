/*
 * Copyright (c) 2015-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.redfish.services.allocation.validation;

import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.services.helpers.VolumeHelper;
import com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive.MasterDrive;
import com.intel.podm.common.types.Protocol;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.math.BigDecimal;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.types.ReplicaType.CLONE;
import static com.intel.podm.common.types.ReplicaType.SNAPSHOT;
import static com.intel.podm.common.utils.Converters.convertBytesToGib;
import static com.intel.podm.common.utils.Converters.convertGibToBytes;
import static java.lang.String.format;
import static java.util.Arrays.asList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class NewRemoteDriveValidator {
    @Inject
    private GenericDao genericDao;

    @Inject
    private RequestedProtocolValidator requestedProtocolValidator;

    @Inject
    private VolumeHelper volumeHelper;

    @Transactional(MANDATORY)
    public Violations validate(RemoteDrive drive) {
        Violations violations = new Violations();

        validateMasterIfPresent(drive, violations);
        if (violations.hasViolations()) {
            return violations;
        }

        if (drive.getCapacityGib() != null) {
            violations.addAll(validateCapacity(drive));
        }
        violations.addAll(validateProtocolFromNewRequestedDrive(drive));

        return violations;
    }

    private Violations validateProtocolFromNewRequestedDrive(RemoteDrive drive) {
        Protocol protocol = drive.getProtocol();
        Violations violations = new Violations();
        if (protocol != null) {
            violations.addAll(requestedProtocolValidator.validate(protocol));
        }
        return violations;
    }

    private void validateMasterIfPresent(RemoteDrive drive, Violations violations) {
        if (isMasterPresent(drive)) {
            violations.addAll(validateMaster(drive));
        }
    }

    private boolean isMasterPresent(RemoteDrive drive) {
        return drive.getMaster() != null;
    }

    private Violations validateMaster(RemoteDrive drive) {
        MasterDrive masterDrive = drive.getMaster();
        Violations violations = new Violations();
        if (masterDrive == null) {
            violations.addMissingPropertyViolation("Master");
        } else {
            if (masterDrive.getType() == null) {
                violations.addMissingPropertyViolation("Master.Type");
            } else if (!CLONE.equals(masterDrive.getType()) && !SNAPSHOT.equals(masterDrive.getType())) {
                violations.addValueNotAllowedViolation("Master.Type", asList(CLONE, SNAPSHOT));
            }
            if (masterDrive.getResourceContext() == null) {
                violations.addMissingPropertyViolation("Master.Resource");
            } else {
                validateMasterAvailabilityAndProperties(drive.getProtocol(), masterDrive, violations);
            }
        }
        return violations;
    }

    private void validateMasterAvailabilityAndProperties(Protocol driveProtocol, MasterDrive masterDrive, Violations violations) {
        try {
            Volume masterVolume = genericDao.find(Volume.class, masterDrive.getResourceContext().getId());

            if (!statusOf(masterVolume).isEnabled().isHealthy().verify()) {
                violations.addViolation("Provided master drive should be enabled and healthy.");
            }

            Protocol masterDriveProtocol = volumeHelper.retrieveProtocolFromVolume(masterVolume);
            if (masterDriveProtocol == NVME_OVER_FABRICS) {
                violations.addViolation("Allocation with Clone or Snapshot are not supported for NVME storage.");
            } else if (driveProtocol != null && driveProtocol != masterDriveProtocol) {
                violations.addViolation("Provided protocol is different than master drive's fabric protocol.");
            }
        } catch (EntityNotFoundException e) {
            violations.addViolation("Provided master drive does not exist.");
        }
    }

    private Violations validateCapacity(RemoteDrive drive) {
        BigDecimal capacity = drive.getCapacityGib();
        Violations violations = new Violations();

        if (capacity.signum() <= 0) {
            violations.addViolation("CapacityGiB should be greater than zero.");
            return violations;
        }

        if (isMasterPresent(drive)) {
            validateThatRequestedDriveIsBigEnough(drive, violations);
        }


        return violations;
    }

    private void validateThatRequestedDriveIsBigEnough(RemoteDrive drive, Violations violations) {
        MasterDrive masterDrive = drive.getMaster();
        if (CLONE.equals(masterDrive.getType())) {
            try {
                Volume masterVolume = genericDao.find(Volume.class, masterDrive.getResourceContext().getId());
                BigDecimal driveCapacityBytes = convertGibToBytes(drive.getCapacityGib());
                BigDecimal masterCapacityBytes = masterVolume.getCapacityBytes();
                if (driveCapacityBytes != null && driveCapacityBytes.compareTo(masterCapacityBytes) < 0) {
                    violations.addViolation(
                        format(
                            "Remote Drive's CapacityGiB [%sGiB] should be greater or equal to Master Drive's CapacityGiB [%sGiB].",
                            drive.getCapacityGib(),
                            convertBytesToGib(masterCapacityBytes)
                        )
                    );
                }
            } catch (EntityNotFoundException e) {
                violations.addViolation("Provided master drive does not exist.");
            }
        }
    }
}
