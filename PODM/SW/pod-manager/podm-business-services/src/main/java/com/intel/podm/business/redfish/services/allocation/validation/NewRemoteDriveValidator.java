/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.entities.dao.RemoteTargetIscsiAddressDao;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.services.redfish.requests.RequestedNode;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.math.BigDecimal;

import static com.intel.podm.common.types.ReplicationMethod.CLONE;
import static com.intel.podm.common.types.VolumeMode.LV;
import static java.lang.String.format;

@Dependent
public class NewRemoteDriveValidator {
    @Inject
    private GenericDao genericDao;

    @Inject
    private RemoteTargetIscsiAddressDao remoteTargetIscsiAddressDao;

    public Violations validate(RequestedNode.RemoteDrive drive) {
        Violations violations = new Violations();

        Violations masterDriveViolations = validateMaster(drive);
        violations.addAll(masterDriveViolations);

        if (violations.hasViolations()) {
            return violations;
        }

        Violations remoteDriveViolations = validateRemoteDrive(drive);
        violations.addAll(remoteDriveViolations);

        return violations;
    }

    private Violations validateMaster(RequestedNode.RemoteDrive drive) {
        RequestedNode.RemoteDrive.MasterDrive masterDrive = drive.getMaster();
        Violations violations = new Violations();
        if (masterDrive == null) {
            violations.addMissingPropertyViolation("Master");
        } else {
            if (masterDrive.getType() == null) {
                violations.addMissingPropertyViolation("Master.Type");
            }
            if (masterDrive.getResourceContext() == null) {
                violations.addMissingPropertyViolation("Master.Resource");
            } else {
                validateMasterAvailabilityAndProperties(masterDrive, violations);
            }
        }
        return violations;
    }

    private void validateMasterAvailabilityAndProperties(RequestedNode.RemoteDrive.MasterDrive masterDrive, Violations violations) {
        try {
            LogicalDrive master = genericDao.find(LogicalDrive.class, masterDrive.getResourceContext().getId());

            if (master.getMode() != LV) {
                String msg = format("Provided master drive has wrong mode: %s, please provide Master Drive with mode %s.", master.getMode(), LV);
                violations.addViolation(msg);
            }

            if (!master.isEnabledAndHealthy()) {
                violations.addViolation("Provided master drive should be enabled and healthy.");
            }
        } catch (EntityNotFoundException e) {
            violations.addViolation("Provided master drive does not exist.");
        }
    }

    private Violations validateRemoteDrive(RequestedNode.RemoteDrive drive) {
        Violations remoteDriveViolations = validateCapacity(drive);
        String iscsiAddress = drive.getIscsiAddress();

        if (iscsiAddress == null) {
            remoteDriveViolations.addMissingPropertyViolation("iSCSIAddress");
        } else if (remoteTargetIscsiAddressDao.getRemoteTargetIscsiAddressesByTargetIqn(iscsiAddress).size() > 0) {
            remoteDriveViolations.addViolation("Target with provided address already exists.");
        }

        return remoteDriveViolations;
    }

    private Violations validateCapacity(RequestedNode.RemoteDrive drive) {
        BigDecimal capacity = drive.getCapacityGib();
        Violations violations = new Violations();

        if (capacity == null) {
            violations.addMissingPropertyViolation("CapacityGiB");
        } else {
            if (capacity.signum() <= 0) {
                violations.addViolation("CapacityGiB should be greater than zero.");
                return violations;
            }

            validateThatRequestedDriveIsBigEnough(drive, violations);
        }

        return violations;
    }

    private void validateThatRequestedDriveIsBigEnough(RequestedNode.RemoteDrive drive, Violations violations) {
        if (drive.getMaster().getType().equals(CLONE)) {
            try {
                LogicalDrive master = genericDao.find(LogicalDrive.class, drive.getMaster().getResourceContext().getId());
                if (drive.getCapacityGib().compareTo(master.getCapacityGib()) < 0) {
                    violations.addViolation(
                        format(
                            "Remote Drive's CapacityGiB [%sGiB] should be greater or equal to Master Drive's CapacityGiB [%sGiB].",
                            drive.getCapacityGib(),
                            master.getCapacityGib()
                        )
                    );
                }
            } catch (EntityNotFoundException e) {
                violations.addViolation("Provided master drive does not exist.");
            }
        }
    }
}
