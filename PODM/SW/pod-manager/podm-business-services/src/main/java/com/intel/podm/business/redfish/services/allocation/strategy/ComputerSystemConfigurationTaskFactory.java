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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.services.assembly.tasks.ComputerSystemConfigurationTask;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.services.redfish.requests.RequestedNode.Security;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.actions.ChangeTpmStatusUpdateDefinition;
import com.intel.podm.common.types.actions.ComputerSystemUpdateDefinition;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static java.lang.Boolean.TRUE;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class ComputerSystemConfigurationTaskFactory {
    @Inject
    private BeanFactory beanFactory;

    @Inject
    private ChangeTpmStateTaskFactory changeTpmStateTaskFactory;

    @Inject
    private TrustedModuleOverseer trustedModuleOverseer;

    private List<NodeTask> nodeTasks = new ArrayList<>();

    @Transactional(REQUIRED)
    public List<NodeTask> createComputerSystemConfigurationTask(Security security, ComputerSystem computerSystem) {
        nodeTasks.add(createComputerSystemConfigurationTask(computerSystem, getComputerSystemUpdateDefinition(computerSystem)));
        if (checkSecurityPropertiesAreSpecified(security)) {
            nodeTasks.add(changeTpmStateTaskFactory.createChangeTpmStateTask(computerSystem, getChangeTpmStatusUpdateDefinition(security, computerSystem)));
        }
        return nodeTasks;
    }

    private ComputerSystemUpdateDefinition getComputerSystemUpdateDefinition(ComputerSystem computerSystem) {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition();
        // TODO consult with architects logic of setting computerSystemUpdateDefinition
        if (computerSystem.getUserModeEnabled() != null) {
            computerSystemUpdateDefinition.setUserModeEnabled(TRUE);
        }
        return computerSystemUpdateDefinition;
    }

    private ComputerSystemConfigurationTask createComputerSystemConfigurationTask(ComputerSystem computerSystem,
                                                                                  ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
        ComputerSystemConfigurationTask computerSystemConfigurationTask = beanFactory.create(ComputerSystemConfigurationTask.class);
        computerSystemConfigurationTask.setComputerSystem(computerSystem);
        computerSystemConfigurationTask.setComputerSystemUpdateDefinition(computerSystemUpdateDefinition);
        return computerSystemConfigurationTask;
    }

    private ChangeTpmStatusUpdateDefinition getChangeTpmStatusUpdateDefinition(Security security, ComputerSystem computerSystem) {
        return trustedModuleOverseer.prepareChangeTpmStateUpdateDefinition(security, computerSystem.getTrustedModules());
    }

    private boolean checkSecurityPropertiesAreSpecified(Security security) {
        return security != null && (TRUE.equals(security.getTpmPresent()) || security.getTpmInterfaceType() != null);
    }
}
