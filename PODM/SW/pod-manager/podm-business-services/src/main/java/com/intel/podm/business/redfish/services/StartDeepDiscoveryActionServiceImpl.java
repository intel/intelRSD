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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.bootcontrol.DeepDiscovery;
import com.intel.podm.business.redfish.services.bootcontrol.DeepDiscoveryException;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.StartDeepDiscoveryRequest;
import com.intel.podm.common.synchronization.TaskCoordinator;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.concurrent.TimeoutException;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
class StartDeepDiscoveryActionServiceImpl implements ActionService<StartDeepDiscoveryRequest> {
    @Inject
    private DeepDiscovery deepDiscovery;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Override
    @Transactional(REQUIRES_NEW)
    public void perform(Context target, StartDeepDiscoveryRequest request) throws BusinessApiException, TimeoutException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(target);
        try {
            taskCoordinator.run(computerSystem.getService().getUuid(), () -> deepDiscovery.triggerForComputerSystem(computerSystem.getId()));
        } catch (DeepDiscoveryException e) {
            throw new ResourceStateMismatchException(e.getMessage(), e);
        }
    }
}
