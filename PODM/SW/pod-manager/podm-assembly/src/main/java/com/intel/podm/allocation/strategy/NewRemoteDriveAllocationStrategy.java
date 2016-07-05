/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.allocation.strategy;

import com.intel.podm.allocation.CompositionException;
import com.intel.podm.allocation.validation.NewRemoteDriveValidator;
import com.intel.podm.allocation.validation.Violations;
import com.intel.podm.assembly.tasks.NewRemoteDriveAssemblyTask;
import com.intel.podm.assembly.tasks.NewRemoteDriveTaskFactory;
import com.intel.podm.assembly.tasks.NodeAssemblyTask;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;
import com.intel.podm.business.entities.redfish.components.ComposedNode;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.google.common.collect.Lists.newArrayList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class NewRemoteDriveAllocationStrategy implements RemoteDriveAllocationStrategy {
    @Inject
    private NewRemoteDriveValidator validator;

    @Inject
    private NewRemoteDriveResourcesFinder finder;

    @Inject
    private NewRemoteDriveTaskFactory newRemoteDriveTaskFactory;

    private RequestedRemoteDrive drive;
    private NewRemoteDriveResourcesFinder.NewRemoteDriveAllocationResources resources;
    private List<NodeAssemblyTask> tasks = newArrayList();

    public void setDrive(RequestedRemoteDrive drive) {
        this.drive = drive;
    }

    @Override
    public Violations validate() {
        return validator.validate(drive);
    }

    @Override
    public Violations findResources() {
        resources = finder.find(drive);
        return resources.getViolations();
    }

    @Override
    public void allocate(ComposedNode composedNode) throws CompositionException {
        reserveLvgSpace(composedNode);
        NewRemoteDriveAssemblyTask task = newRemoteDriveTaskFactory.create(resources.getLvg().getId(),
                resources.getMaster().getSourceUri(),
                drive);
        tasks.add(task);
    }

    @Override
    public List<NodeAssemblyTask> getTasks() {
        return tasks;
    }

    private void reserveLvgSpace(ComposedNode composedNode) {
        composedNode.setRemoteDriveCapacityGib(resources.getCapacity());
        resources.getLvg().addComposedNode(composedNode);
    }
}
