/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.ResourceStateMismatchException;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.AssemblyException;
import com.intel.rsd.nodecomposer.composition.assembly.NodeAssembler;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.utils.retry.RetryOnRollback;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ALLOCATED;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class AssemblyActionService {
    private final NodeAssembler nodeAssembler;
    private final EntityTreeTraverser traverser;

    @Autowired
    public AssemblyActionService(NodeAssembler nodeAssembler, EntityTreeTraverser traverser) {
        this.nodeAssembler = nodeAssembler;
        this.traverser = traverser;
    }

    @RetryOnRollback(3)
    @Transactional(value = REQUIRES_NEW, rollbackOn = BusinessApiException.class)
    public void perform(ODataId composedNodeODataId) throws BusinessApiException {
        ComposedNode composedNode = traverser.traverseComposedNode(composedNodeODataId);
        try {
            if (!composedNode.isInAnyOfStates(ALLOCATED)) {
                throw new ResourceStateMismatchException(format("Only composed node in %s state can be assembled", ALLOCATED));
            }
            nodeAssembler.assemble(composedNode.getUri());
        } catch (AssemblyException e) {
            throw new EntityOperationException("Assembly failed: " + e.getMessage(), e);
        }
    }
}
