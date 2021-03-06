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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

@Component
public class InitiatorEndpointAssemblyTaskFactory {
    private final NodeComposerBeanFactory beanFactory;

    @Autowired
    public InitiatorEndpointAssemblyTaskFactory(NodeComposerBeanFactory beanFactory) {
        this.beanFactory = beanFactory;
    }

    public InitiatorEndpointAssemblyTask create(ODataId fabricOdataId) {
        requiresNonNull(fabricOdataId, "fabricOdataId");
        val initiatorEndpointAssemblyTask = beanFactory.create(InitiatorEndpointAssemblyTask.class);
        initiatorEndpointAssemblyTask.setFabricOdataId(fabricOdataId);
        return initiatorEndpointAssemblyTask;
    }
}
