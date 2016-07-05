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

import com.intel.podm.allocation.strategy.matcher.ComputerSystemMatcher;
import com.intel.podm.allocation.validation.ComputerSystemAllocationValidator;
import com.intel.podm.allocation.validation.Violations;
import com.intel.podm.assembly.tasks.NodeAssemblyTask;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.ComposedNodeState.ALLOCATING;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.NodeSystemType.LOGICAL;
import static com.intel.podm.common.types.State.ENABLED;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ComputerSystemAllocationStrategy {
    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private GenericDao genericDao;

    @Inject
    private ChassisDao chassisDao;

    @Inject
    private ComputerSystemAllocationValidator validator;

    @Inject
    private ComputerSystemMatcher computerSystemMatcher;

    @Inject
    private EthernetInterfacesAllocator ethernetInterfacesAllocator;

    private RequestedNode requestedNode;
    private List<NodeAssemblyTask> tasks = newArrayList();

    public void setRequestedNode(RequestedNode requestedNode) {
        this.requestedNode = requestedNode;
    }

    public Violations validate() {
        return validator.validate(requestedNode);
    }

    public ComputerSystem findResources() throws ResourceFinderException {
        List<ComputerSystem> computerSystems = computerSystemDao.getNotAllocatedComputerSystems();
        computerSystems = computerSystemMatcher.matches(requestedNode, computerSystems);
        return computerSystems.iterator().next();
    }

    public ComposedNode allocateWithComputerSystem(ComputerSystem computerSystem) {
        List<NodeAssemblyTask> allocationTasks =
                ethernetInterfacesAllocator.allocate(computerSystem.getEthernetInterfaces(), requestedNode.getEthernetInterfaces());

        ComposedNode composedNode = createComposedNodeWithComputerSystem(computerSystem);
        tasks.addAll(allocationTasks);

        return composedNode;
    }

    private ComposedNode createComposedNodeWithComputerSystem(ComputerSystem computerSystem) {
        ComposedNode composedNode = createComposedNode();
        composedNode.setComputerSystem(computerSystem);
        computerSystem.setAllocated(true);
        computerSystem.getEthernetInterfaces().forEach(composedNode::addEthernetInterface);
        computerSystem.getAdapters().forEach(adapter -> adapter.getDevices().forEach(composedNode::addLocalDrive));
        computerSystem.getSimpleStorages().forEach(composedNode::addSimpleStorage);

        return composedNode;
    }

    private ComposedNode createComposedNode() {
        ComposedNode composedNode = genericDao.create(ComposedNode.class);

        composedNode.setName(requestedNode.getName());
        composedNode.setSystemType(LOGICAL);
        composedNode.setDescription(requestedNode.getDescription());
        composedNode.setComposedNodeState(ALLOCATING);
        composedNode.setStatus(new Status(ENABLED, OK, OK));

        return composedNode;
    }

    public List<NodeAssemblyTask> getTasks() {
        return tasks;
    }
}
