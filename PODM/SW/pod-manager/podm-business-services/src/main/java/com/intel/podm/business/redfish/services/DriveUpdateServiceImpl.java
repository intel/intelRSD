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
import com.intel.podm.business.redfish.ServiceTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.UpdateService;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.redfish.RedfishDrive;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.concurrent.TimeoutException;

@Dependent
public class DriveUpdateServiceImpl implements UpdateService<RedfishDrive> {
    @Inject
    private ServiceTraverser traverser;

    @Inject
    private DriveActionsService driveActionsService;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Override
    public void perform(Context target, RedfishDrive representation) throws BusinessApiException, TimeoutException {
        taskCoordinator.runThrowing(traverser.traverseServiceUuid(target), () -> driveActionsService.updateDrive(target, representation));
    }
}
