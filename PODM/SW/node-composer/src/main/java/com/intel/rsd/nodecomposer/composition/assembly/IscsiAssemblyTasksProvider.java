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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.PatchEndpointsAssemblyTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.PatchEndpointsAssemblyTaskFactory;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.PatchNetworkDeviceFunctionAssemblyTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.PatchNetworkDeviceFunctionAssemblyTaskFactory;
import com.intel.rsd.nodecomposer.types.Chap;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.List;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class IscsiAssemblyTasksProvider {
    private final PatchNetworkDeviceFunctionAssemblyTaskFactory patchNetworkDeviceFunctionAssemblyTaskFactory;
    private final PatchEndpointsAssemblyTaskFactory patchEndpointsAssemblyTaskFactory;
    private final ChapConfigurator chapConfigurator;

    @Autowired
    public IscsiAssemblyTasksProvider(PatchNetworkDeviceFunctionAssemblyTaskFactory patchNetworkDeviceFunctionAssemblyTaskFactory,
                                      PatchEndpointsAssemblyTaskFactory patchEndpointsAssemblyTaskFactory, ChapConfigurator chapConfigurator) {
        this.patchNetworkDeviceFunctionAssemblyTaskFactory = patchNetworkDeviceFunctionAssemblyTaskFactory;
        this.patchEndpointsAssemblyTaskFactory = patchEndpointsAssemblyTaskFactory;
        this.chapConfigurator = chapConfigurator;
    }

    public List<NodeTask> createTasks(boolean needToPatchNetworkDeviceFunction) {
        List<NodeTask> tasks = new ArrayList<>();
        Chap chap = chapConfigurator.generateChap();

        PatchEndpointsAssemblyTask patchEndpointsAssemblyTask = patchEndpointsAssemblyTaskFactory.create();
        patchEndpointsAssemblyTask.setChap(chap);
        tasks.add(patchEndpointsAssemblyTask);

        if (needToPatchNetworkDeviceFunction) {
            PatchNetworkDeviceFunctionAssemblyTask patchNetworkDeviceFunctionAssemblyTask = patchNetworkDeviceFunctionAssemblyTaskFactory.create();
            patchNetworkDeviceFunctionAssemblyTask.setChap(chap);
            tasks.add(patchNetworkDeviceFunctionAssemblyTask);
        }

        return tasks;
    }
}
