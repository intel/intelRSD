/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.HasProtocol;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.allocation.AllocationRequestProcessingException;
import com.intel.podm.business.redfish.services.helpers.VolumeHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.podm.common.types.Protocol;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.EnumeratedType.LOGGER;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class ExistingRemoteDriveValidator {
    @Inject
    private VolumeHelper volumeHelper;

    @Inject
    private RequestedProtocolValidator protocolValidator;

    @Inject
    private EntityTreeTraverser traverser;

    @Transactional(MANDATORY)
    public Violations validateProtocolFromExistingRequestedDrive(RemoteDrive drive) {
        Protocol protocol = drive.getProtocol();
        Violations violations = new Violations();
        if (protocol != null) {
            violations.addAll(protocolValidator.validate(protocol));
            violations.addAll(validateIfRequestedResourceMatchToRequestedProtocol(protocol, drive.getResourceContext()));
            violations.addAll(validateCapacity(drive));
        }
        return violations;
    }

    public void validateRemoteDrive(RemoteDrive resource) throws AllocationRequestProcessingException {
        Violations violations = new Violations();
        if (resource == null || resource.getResourceContext() == null) {
            violations.addMissingPropertyViolation("resource");
            throw new AllocationRequestProcessingException(violations);
        }
    }

    private Violations validateCapacity(RemoteDrive resource) {
        Violations violations = new Violations();

        if (resource.getCapacityGib() != null) {
            violations.addViolation("CapacityGiB property is not allowed when requesting Composed Node with existing Volume or Endpoint");
        }
        return violations;
    }

    private Violations validateIfRequestedResourceMatchToRequestedProtocol(Protocol protocol, Context resourceContext) {
        Violations violations = new Violations();

        final Entity resourceEntity;
        try {
            resourceEntity = traverser.traverse(resourceContext);
        } catch (ContextResolvingException e) {
            LOGGER.e("Requested resource does not exist." + e);
            violations.addViolation("Requested resource does not exist.");
            return violations;
        }

        violations.addAll(validateProtocolWithResource(protocol, resourceEntity));
        return violations;
    }

    private Violations validateProtocolWithResource(Protocol protocol, Entity resourceEntity) {
        Violations violations = new Violations();
        violations.addAll(validateProtocolWithEntityContainingRemoteDriveProtocol(protocol, resourceEntity));
        violations.addAll(validateProtocolWithVolume(protocol, resourceEntity));

        return violations;
    }

    private Violations validateProtocolWithEntityContainingRemoteDriveProtocol(Protocol protocol, Entity resourceEntity) {
        if (resourceEntity instanceof HasProtocol && protocol != ((HasProtocol) resourceEntity).getProtocol()) {
            return Violations.createWithViolations("Requested protocol did not match to protocol in resource");
        }
        return new Violations();
    }

    private Violations validateProtocolWithVolume(Protocol protocol, Entity resourceEntity) {
        if (resourceEntity instanceof Volume && protocol != volumeHelper.retrieveProtocolFromVolume((Volume) resourceEntity)) {
            return Violations.createWithViolations("Requested protocol did not match to protocol in resource");
        }
        return new Violations();
    }
}
