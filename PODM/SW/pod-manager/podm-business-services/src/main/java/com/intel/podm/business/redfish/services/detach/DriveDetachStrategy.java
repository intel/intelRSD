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

package com.intel.podm.business.redfish.services.detach;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.redfish.ServiceTraverser;
import com.intel.podm.business.redfish.services.PcieDriveDetacher;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.concurrent.TimeoutException;

@Dependent
public class DriveDetachStrategy implements DetachResourceStrategy {
    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private ServiceTraverser traverser;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Inject
    private PcieDriveDetacher detacher;

    @Override
    public void detach(Context composedNodeContext, Context resourceToDetach) throws BusinessApiException, TimeoutException {
        taskCoordinator.run(traverser.traverseServiceUuid(resourceToDetach), () -> detacher.detachDriveFromZone(composedNodeContext, resourceToDetach));
        taskCoordinator.run(serviceConfigHolder.get().getUuid(), () -> detacher.detachDriveFromNode(composedNodeContext, resourceToDetach));
    }
}
