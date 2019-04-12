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

package com.intel.rsd.nodecomposer.composition.allocation;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.AllocationStrategyFactory;
import com.intel.rsd.nodecomposer.composition.allocation.validation.RequestedNodeResourceContextsValidator;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTasksCoordinator;
import com.intel.rsd.nodecomposer.utils.retry.RetryOnRollback;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class NodeAllocator {
    private final NodeTasksCoordinator nodeTasksCoordinator;
    private final AllocationStrategyFactory allocationStrategyFactory;
    private final RequestedNodeResourceContextsValidator requestedNodeResourceContextsValidator;

    @Autowired
    public NodeAllocator(NodeTasksCoordinator nodeTasksCoordinator,
                         AllocationStrategyFactory allocationStrategyFactory,
                         RequestedNodeResourceContextsValidator requestedNodeResourceContextsValidator) {

        this.nodeTasksCoordinator = nodeTasksCoordinator;
        this.allocationStrategyFactory = allocationStrategyFactory;
        this.requestedNodeResourceContextsValidator = requestedNodeResourceContextsValidator;
    }

    @RetryOnRollback(10)
    @Transactional(value = REQUIRES_NEW, rollbackOn = CompositionException.class)
    public ODataId allocate(RequestedNode requestedNode) throws CompositionException {
        requestedNodeResourceContextsValidator.validateExistenceOfIncludedResources(requestedNode);
        val allocationContext = allocationStrategyFactory.create(requestedNode).allocate(requestedNode);
        nodeTasksCoordinator.register(allocationContext);
        return allocationContext.getODataId();
    }
}
