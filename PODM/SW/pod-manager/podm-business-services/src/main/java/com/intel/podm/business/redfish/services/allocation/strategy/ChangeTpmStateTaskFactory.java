/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.services.assembly.tasks.ChangeTpmStateTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.services.redfish.requests.RequestedNode.Security;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class ChangeTpmStateTaskFactory {
    @Inject
    private BeanFactory beanFactory;

    @Inject
    private TrustedModuleOverseer trustedModuleOverseer;

    @Transactional(REQUIRED)
    public NodeTask createChangeTpmStateTask(Security security, ComputerSystem computerSystem) {
        ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition =
            trustedModuleOverseer.prepareChangeTpmStateUpdateDefinition(security, computerSystem.getTrustedModules());
        return prepareChangeTpmStateTask(computerSystem, changeTpmStatusUpdateDefinition);
    }

    private ChangeTpmStateTask prepareChangeTpmStateTask(ComputerSystem computerSystem,
                                                         ChangeTpmStatusUpdateDefinition changeTpmStatusUpdateDefinition) {
        ChangeTpmStateTask changeTpmStateTask = beanFactory.create(ChangeTpmStateTask.class);
        changeTpmStateTask.setComputerSystem(computerSystem);
        changeTpmStateTask.setChangeTpmStatusUpdateDefinition(changeTpmStatusUpdateDefinition);
        return changeTpmStateTask;
    }
}
