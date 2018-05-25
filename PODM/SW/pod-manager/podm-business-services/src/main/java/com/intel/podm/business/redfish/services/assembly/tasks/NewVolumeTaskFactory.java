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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.redfish.services.allocation.strategy.RemoteDriveAllocationContextDescriptor;
import com.intel.podm.business.redfish.services.assembly.tasks.volume.CreateVolumeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.volume.NewVolumeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.volume.ObtainNewVolumeTask;
import com.intel.podm.business.redfish.services.assembly.tasks.volume.WaitForVolumeTask;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.LinkedList;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class NewVolumeTaskFactory {
    @Inject
    private BeanFactory beanFactory;

    @Transactional(MANDATORY)
    public List<NodeTask> createTasks(RemoteDriveAllocationContextDescriptor resourceDescriptor) {
        List<NodeTask> tasks = new LinkedList<>();

        NewVolumeTask createVolumeTask = beanFactory.create(CreateVolumeTask.class);
        createVolumeTask.setResourceDescriptor(resourceDescriptor);
        tasks.add(createVolumeTask);

        NewVolumeTask waitForVolumeTask = beanFactory.create(WaitForVolumeTask.class);
        waitForVolumeTask.setResourceDescriptor(resourceDescriptor);
        tasks.add(waitForVolumeTask);

        NewVolumeTask obtainNewVolumeTask = beanFactory.create(ObtainNewVolumeTask.class);
        obtainNewVolumeTask.setResourceDescriptor(resourceDescriptor);
        tasks.add(obtainNewVolumeTask);

        return tasks;
    }
}
