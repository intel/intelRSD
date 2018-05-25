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

package com.intel.podm.business.redfish.services.attach.strategy;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.ServiceTraverser;
import com.intel.podm.business.redfish.services.VolumeActionsService;
import com.intel.podm.business.redfish.services.attach.AttachResourceStrategy;
import com.intel.podm.business.redfish.services.helpers.EndpointFinder;
import com.intel.podm.business.redfish.services.helpers.VolumeHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.redfish.requests.AttachResourceRequest;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.Protocol;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.Violations.createWithViolations;
import static com.intel.podm.business.services.context.ContextType.VOLUME;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class VolumeAttachStrategy implements AttachResourceStrategy {
    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private ServiceTraverser traverser;

    @Inject
    private EntityTreeTraverser entityTraverser;

    @Inject
    private VolumeActionsService volumeActionsService;

    @Inject
    private VolumeHelper volumeHelper;

    @Inject
    private EndpointFinder endpointFinder;

    @Override
    public ContextType supportedType() {
        return VOLUME;
    }

    @Override
    @Transactional(MANDATORY)
    public void attach(Context target, Context resourceContext) throws BusinessApiException, TimeoutException {
        taskCoordinator.run(traverser.traverseServiceUuid(resourceContext), () -> volumeActionsService.attachVolume(target, resourceContext));
    }

    @Override
    @Transactional(MANDATORY)
    public void validate(Context target, AttachResourceRequest request) throws BusinessApiException {
        Volume volume = (Volume) entityTraverser.traverse(request.getResourceContext());
        validateVolumeAttachAbility(volume);
        validateVolumeProtocol(request.getProtocol(), volume);
        validateVolumeEndpoints(volume);
    }

    private void validateVolumeEndpoints(Volume volume) throws RequestValidationException {
        if (!volume.getEndpoints().isEmpty()) {
            if (endpointFinder.getAttachableEndpoints(volume).size() <= 0) {
                throw new RequestValidationException(createWithViolations("All endpoints attached to selected volume are in use."));
            }
        }
    }

    private void validateVolumeAttachAbility(Volume volume) throws RequestValidationException {
        if (volume.getMetadata().isAllocated()) {
            throw new RequestValidationException(createWithViolations("Selected Volume is currently in use!"));
        }
    }

    private void validateVolumeProtocol(Protocol protocol, Volume volume) throws RequestValidationException {
        Protocol protocolFromVolume = volumeHelper.retrieveProtocolFromVolume(volume);
        if (!NVME_OVER_FABRICS.equals(protocolFromVolume)) {
            throw new RequestValidationException(createWithViolations("Only Volumes from NVMeOF protocol are allowed to attach"));
        }

        if (protocol != null && !protocol.equals(protocolFromVolume)) {
            throw new RequestValidationException(createWithViolations(format("Volume protocol does not match chosen protocol: %s", protocol)));
        }
    }
}
