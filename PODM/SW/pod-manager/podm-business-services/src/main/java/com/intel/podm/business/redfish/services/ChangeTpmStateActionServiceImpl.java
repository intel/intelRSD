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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.allocation.strategy.ChangeTpmStateTaskFactory;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.ChangeTpmStateRequest;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.concurrent.TimeoutException;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
public class ChangeTpmStateActionServiceImpl implements ActionService<ChangeTpmStateRequest> {
    @Inject
    private ChangeTpmStateTaskFactory changeTpmStateTaskFactory;
    @Inject
    private EntityTreeTraverser traverser;
    @Inject
    private TaskCoordinator taskCoordinator;

    @Override
    @Transactional(REQUIRES_NEW)
    public void perform(Context context, ChangeTpmStateRequest request) throws BusinessApiException, TimeoutException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(context);
        // synchronization on computerSystem.getUuid() in case of long-term operation
        taskCoordinator.run(computerSystem.getUuid(),
            changeTpmStateTaskFactory.createChangeTpmStateTask(computerSystem, definitionFromRequest(request)));
    }

    private ChangeTpmStatusUpdateDefinition definitionFromRequest(ChangeTpmStateRequest request) {
        return new ChangeTpmStatusUpdateDefinition(request.getDeviceEnabled(), request.getInterfaceType(), request.getClearOwnership());
    }
}
