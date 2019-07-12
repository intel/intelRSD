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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode.Security;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.ComputerSystemConfigurationTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.types.actions.ChangeTpmStatusUpdateDefinition;
import com.intel.rsd.nodecomposer.types.actions.ComputerSystemUpdateDefinition;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static java.lang.Boolean.TRUE;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class ComputerSystemConfigurationTaskFactory {
    private final NodeComposerBeanFactory beanFactory;
    private final ChangeTpmStateTaskFactory changeTpmStateTaskFactory;
    private final TrustedModuleOverseer trustedModuleOverseer;

    private List<NodeTask> nodeTasks = new ArrayList<>();

    @Autowired
    public ComputerSystemConfigurationTaskFactory(NodeComposerBeanFactory beanFactory, ChangeTpmStateTaskFactory changeTpmStateTaskFactory,
                                                  TrustedModuleOverseer trustedModuleOverseer) {
        this.beanFactory = beanFactory;
        this.changeTpmStateTaskFactory = changeTpmStateTaskFactory;
        this.trustedModuleOverseer = trustedModuleOverseer;
    }

    @Transactional(REQUIRED)
    public List<NodeTask> createComputerSystemConfigurationTask(Security security, ComputerSystem computerSystem) {
        if (computerSystem.getUserModeEnabled() != null) {
            nodeTasks.add(createComputerSystemConfigurationTask(computerSystem, getComputerSystemUpdateDefinition()));
        }
        if (checkSecurityPropertiesAreSpecified(security)) {
            nodeTasks.add(changeTpmStateTaskFactory.createChangeTpmStateTask(
                computerSystem.getUri(), getChangeTpmStatusUpdateDefinition(security, computerSystem))
            );
        }
        return nodeTasks;
    }

    private ComputerSystemUpdateDefinition getComputerSystemUpdateDefinition() {
        ComputerSystemUpdateDefinition computerSystemUpdateDefinition = new ComputerSystemUpdateDefinition();
        computerSystemUpdateDefinition.setUserModeEnabled(TRUE);
        return computerSystemUpdateDefinition;
    }

    private ComputerSystemConfigurationTask createComputerSystemConfigurationTask(ComputerSystem computerSystem,
                                                                                  ComputerSystemUpdateDefinition computerSystemUpdateDefinition) {
        ComputerSystemConfigurationTask computerSystemConfigurationTask = beanFactory.create(ComputerSystemConfigurationTask.class);
        computerSystemConfigurationTask.setComputerSystemODataId(computerSystem.getUri());
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
