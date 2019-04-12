/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class VlanNodeAssemblyTaskFactory {
    private final NodeComposerBeanFactory beanFactory;

    @Autowired
    public VlanNodeAssemblyTaskFactory(NodeComposerBeanFactory beanFactory) {
        this.beanFactory = beanFactory;
    }

    public NodeTask createVlanCreationTask(EthernetInterface availableEthernetInterface,
                                           RequestedNode.EthernetInterface requestedEthernetInterface) {
        VlanNodeAssemblyTask vlanNodeAssemblyTask = beanFactory.create(VlanNodeAssemblyTask.class);
        vlanNodeAssemblyTask.setRequestedInterface(requestedEthernetInterface);
        vlanNodeAssemblyTask.setAvailableInterfaceODataId(availableEthernetInterface.getUri());

        return vlanNodeAssemblyTask;
    }
}
