/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.RequestValidationException;
import com.intel.podm.business.redfish.ServiceTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.AttachEndpointRequest;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.business.Violations.createWithViolations;
import static javax.transaction.Transactional.TxType.NEVER;

@RequestScoped
class AttachEndpointActionServiceImpl implements ActionService<AttachEndpointRequest> {
    @Inject
    private ServiceTraverser traverser;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Inject
    private PcieZoneAttacher zoneAttacher;

    @Inject
    private PcieDriveAttacher driveAttacher;

    @Override
    @Transactional(NEVER)
    public void perform(Context target, AttachEndpointRequest request) throws BusinessApiException, TimeoutException {
        validateRequest(request);

        final Context pcieDriveContext = getPcieDriveContext(target, request);
        taskCoordinator.run(traverser.traverseServiceUuid(pcieDriveContext), () -> zoneAttacher.attachEndpointToZone(target, pcieDriveContext));
        taskCoordinator.run(serviceConfigHolder.get().getUuid(), () -> driveAttacher.attachPcieDriveToNode(target, pcieDriveContext));
    }

    private void validateRequest(AttachEndpointRequest request) throws RequestValidationException {
        if (request.getCapacityGiB() == null && request.getDriveContext() == null) {
            throw new RequestValidationException(createWithViolations("CapacityGiB or Resource need to be specified"));
        }

        if (request.getCapacityGiB() != null && request.getDriveContext() != null) {
            throw new RequestValidationException(createWithViolations("CapacityGiB and Resource can not be specified at the same time"));
        }
    }

    private Context getPcieDriveContext(Context target, AttachEndpointRequest request) throws BusinessApiException {
        return (request.getDriveContext() != null)
            ? request.getDriveContext()
            : driveAttacher.determinePcieDriveContext(target, request.getCapacityGiB());
    }
}
