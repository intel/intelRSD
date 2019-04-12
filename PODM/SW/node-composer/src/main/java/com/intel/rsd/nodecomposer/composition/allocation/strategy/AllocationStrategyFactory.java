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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationStrategy;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.SetComposedNodeStateToAssembledTask;
import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Arrays;

import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class AllocationStrategyFactory {

    private final ComputerSystemAllocationStrategyFactory computerSystemAllocationStrategyFactory;
    private final RemoteDriveAllocationStrategyFactory remoteDriveStrategyFactory;
    private final NodeComposerBeanFactory beanFactory;
    private ComposedNodeDao composedNodeDao;

    @Autowired
    public AllocationStrategyFactory(ComputerSystemAllocationStrategyFactory computerSystemAllocationStrategyFactory,
                                     RemoteDriveAllocationStrategyFactory remoteDriveStrategyFactory,
                                     NodeComposerBeanFactory beanFactory,
                                     ComposedNodeDao composedNodeDao) {

        this.computerSystemAllocationStrategyFactory = computerSystemAllocationStrategyFactory;
        this.remoteDriveStrategyFactory = remoteDriveStrategyFactory;
        this.composedNodeDao = composedNodeDao;
        this.beanFactory = beanFactory;
    }

    /**
     * @return validate allocation strategy
     * @throws AllocationRequestProcessingException if created strategy is not valid
     */
    @Transactional(MANDATORY)
    public NodeAllocationStrategy create(RequestedNode requestedNode) throws AllocationRequestProcessingException {

        val computerSystemAllocationStrategy = computerSystemAllocationStrategyFactory.create(requestedNode);
        val driveAllocationStrategy = remoteDriveStrategyFactory.create(requestedNode);

        validate(computerSystemAllocationStrategy, driveAllocationStrategy);

        val allocationStrategy = new NodeAllocationStrategy(
            composedNodeDao,
            beanFactory.create(SetComposedNodeStateToAssembledTask.class),
            computerSystemAllocationStrategy,
            driveAllocationStrategy
        );


        return allocationStrategy;
    }

    private void validate(AllocationStrategy... allocationStrategies) throws AllocationRequestProcessingException {
        val violations = Arrays.stream(allocationStrategies)
            .map(AllocationStrategy::validate)
            .reduce(new Violations(), Violations::addAll);

        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }
    }
}
