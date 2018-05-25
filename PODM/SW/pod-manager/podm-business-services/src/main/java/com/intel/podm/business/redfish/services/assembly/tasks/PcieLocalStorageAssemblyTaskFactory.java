/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class PcieLocalStorageAssemblyTaskFactory {
    @Inject
    private BeanFactory beanFactory;

    public NodeTask createLocalStorageTask(Id zoneId, Id endpointId) {
        PcieLocalStorageAssemblyTask pcieLocalStorageAssemblyTask = beanFactory.create(PcieLocalStorageAssemblyTask.class);
        pcieLocalStorageAssemblyTask.setZoneId(zoneId);
        pcieLocalStorageAssemblyTask.setEndpointId(endpointId);
        return pcieLocalStorageAssemblyTask;
    }
}
