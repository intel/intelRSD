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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.services.allocation.AllocationRequestProcessingException;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class AllocationStrategy {
    private ComputerSystemAllocationStrategy computerSystemAllocationStrategy;
    private RemoteDriveAllocationStrategy driveAllocationStrategy;

    public void setComputerSystemAllocationStrategy(ComputerSystemAllocationStrategy computerSystemAllocationStrategy) {
        this.computerSystemAllocationStrategy = computerSystemAllocationStrategy;
    }

    public void setRemoteDriveAllocationStrategy(RemoteDriveAllocationStrategy remoteDriveAllocationStrategy) {
        this.driveAllocationStrategy = remoteDriveAllocationStrategy;
    }

    public void validate() throws AllocationRequestProcessingException {
        Violations violations = computerSystemAllocationStrategy.validate();
        violations.addAll(driveAllocationStrategy.validate());
        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }
    }

    public ComputerSystem findComputerSystemResource() throws ResourceFinderException {
        Violations violations = new Violations();

        ComputerSystem computerSystem = computerSystemAllocationStrategy.findResources();
        violations.addAll(driveAllocationStrategy.findResources());

        if (violations.hasViolations()) {
            throw new ResourceFinderException(violations);
        }

        return computerSystem;
    }

    public ComposedNode allocateWithComputerSystem(ComputerSystem computerSystem) {
        ComposedNode node = computerSystemAllocationStrategy.allocateWithComputerSystem(computerSystem);
        driveAllocationStrategy.allocate(node);

        return node;
    }

    public List<NodeTask> getTasks() {
        List<NodeTask> tasks = new ArrayList<>();
        tasks.addAll(computerSystemAllocationStrategy.getTasks());
        tasks.addAll(driveAllocationStrategy.getTasks());
        return tasks;
    }
}
