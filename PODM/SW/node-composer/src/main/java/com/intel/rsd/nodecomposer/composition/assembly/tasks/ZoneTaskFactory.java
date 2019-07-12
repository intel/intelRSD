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
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class ZoneTaskFactory {
    private final NodeComposerBeanFactory beanFactory;

    @Autowired
    public ZoneTaskFactory(NodeComposerBeanFactory beanFactory) {
        this.beanFactory = beanFactory;
    }

    public ZoneAssemblyTask create(ODataId fabricOdataId) {
        ZoneAssemblyTask zoneAssemblyTask = beanFactory.create(ZoneAssemblyTask.class);
        zoneAssemblyTask.setFabricOdataId(fabricOdataId);
        return zoneAssemblyTask;
    }
}
