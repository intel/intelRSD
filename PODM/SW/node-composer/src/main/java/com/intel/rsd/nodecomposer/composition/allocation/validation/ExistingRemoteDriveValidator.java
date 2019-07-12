/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.validation;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.VolumeHelper;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.HasProtocol;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Slf4j
@Component
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class ExistingRemoteDriveValidator {
    private final VolumeHelper volumeHelper;
    private final RequestedProtocolValidator protocolValidator;
    private final EntityTreeTraverser traverser;

    @Autowired
    public ExistingRemoteDriveValidator(VolumeHelper volumeHelper, RequestedProtocolValidator protocolValidator, EntityTreeTraverser traverser) {
        this.volumeHelper = volumeHelper;
        this.protocolValidator = protocolValidator;
        this.traverser = traverser;
    }

    public Violations validateProtocolFromExistingRequestedDrive(RemoteDrive drive) {
        val protocol = drive.getProtocol();
        val violations = new Violations();
        if (protocol != null) {
            violations.addAll(protocolValidator.validate(protocol));
            violations.addAll(validateIfRequestedResourceMatchToRequestedProtocol(protocol, drive.getResourceODataId()));
            violations.addAll(validateCapacity(drive));
        }
        return violations;
    }

    public void validateRemoteDrive(RemoteDrive resource) throws AllocationRequestProcessingException {
        val violations = new Violations();
        if (resource == null || resource.getResourceODataId() == null) {
            violations.addMissingPropertyViolation("resource");
            throw new AllocationRequestProcessingException(violations);
        }
    }

    private Violations validateCapacity(RemoteDrive resource) {
        val violations = new Violations();

        if (resource.getCapacityGib() != null) {
            violations.addViolation("CapacityGiB property is not allowed when requesting Composed Node with existing Volume or Endpoint");
        }
        return violations;
    }

    private Violations validateIfRequestedResourceMatchToRequestedProtocol(Protocol protocol, ODataId resourceODataId) {
        val violations = new Violations();

        try {
            val resourceEntity = traverser.traverseDiscoverableEntity(resourceODataId);
            violations.addAll(validateProtocolWithResource(protocol, resourceEntity));
        } catch (ODataIdResolvingException e) {
            log.error("Requested resource does not exist." + e);
            violations.addViolation("Requested resource does not exist.");
            return violations;
        }

        return violations;
    }

    private Violations validateProtocolWithResource(Protocol protocol, Entity resourceEntity) {
        val violations = new Violations();
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
