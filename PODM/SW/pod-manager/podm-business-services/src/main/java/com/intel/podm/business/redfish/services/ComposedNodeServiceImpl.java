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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.BootSourceOverrideInvoker;
import com.intel.podm.actions.ComposedNodeReset;
import com.intel.podm.allocation.CompositionException;
import com.intel.podm.allocation.NodeAllocator;
import com.intel.podm.assembly.AssemblyException;
import com.intel.podm.assembly.DisassemblyException;
import com.intel.podm.assembly.NodeAssembler;
import com.intel.podm.assembly.NodeDisassembler;
import com.intel.podm.assembly.tasks.NodeTasksCoordinator;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComposedNodeDto;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.business.redfish.services.helpers.ComposedNodeValidator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ComposedNodeService;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.actions.ResetType;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPOSED_NODE;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@Interceptors(RetryOnRollbackInterceptor.class)
public class ComposedNodeServiceImpl implements ComposedNodeService {
    @Inject
    private ComposedNodeReader composedNodeReader;

    @Inject
    private NodeAllocator nodeAllocator;

    @Inject
    private NodeAssembler nodeAssembler;

    @Inject
    private NodeDisassembler nodeDisassembler;

    @Inject
    private BootSourceOverrideInvoker bootSourceOverrideInvoker;

    @Inject
    private GenericDao genericDao;

    @Inject
    private ComposedNodeReset nodeReset;

    @Inject
    private ComposedNodeValidator composedNodeValidator;

    @Inject
    private NodeTasksCoordinator nodeTasksCoordinator;

    @Inject
    private Logger logger;

    @Override
    @Transactional(REQUIRED)
    public CollectionDto getComposedNodeCollection() {
        return composedNodeReader.getComposedNodeCollection();
    }

    @Override
    @Transactional(REQUIRED)
    public ComposedNodeDto getComposedNode(Context composedNodeContext) throws EntityNotFoundException {
        return composedNodeReader.getComposedNode(composedNodeContext);
    }

    @Override
    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public Context createComposedNode(RequestedNode requestedNode) throws EntityOperationException {
        try {
            composedNodeValidator.validateExistenceOfIncludedResources(requestedNode);
            Id composedNodeId = nodeAllocator.compose(requestedNode).getId();
            nodeAllocator.allocate(composedNodeId);
            return contextOf(composedNodeId, COMPOSED_NODE);
        } catch (CompositionException e) {
            String msg = "Creation failed due to allocation failure: " + e.getMessage();
            throw new EntityOperationException(msg, e);
        }
    }

    @Override
    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void assembleComposedNode(Context composedNodeContext) throws EntityNotFoundException, EntityOperationException {
        ComposedNode composedNode = composedNodeReader.getComposedNodeByContext(composedNodeContext);
        try {
            nodeAssembler.assemble(composedNode);
        } catch (AssemblyException e) {
            String msg = "Assembly failed: " + e.getMessage();
            logger.d(msg, e);
            throw new EntityOperationException(msg, e);
        }
    }

    @Override
    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void reset(Context composedNodeContext, ResetType resetType) throws EntityNotFoundException, EntityOperationException {
        ComposedNode composedNode = composedNodeReader.getComposedNodeByContext(composedNodeContext);
        try {
            nodeReset.reset(composedNode, resetType);
        } catch (ActionException e) {
            logger.d("Reset action failed: error: {}, details: {}", e.getErrorResponse(), e.getMessage());
            throw new EntityOperationException("Reset action failed: " + e.getMessage(), e);
        }
    }

    @Override
    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void overrideBootSource(Context composedNodeContext, BootSourceType type, BootSourceState state)
            throws EntityNotFoundException, EntityOperationException, IllegalStateException {
        ComposedNode composedNode = composedNodeReader.getComposedNodeByContext(composedNodeContext);

        try {
            if (composedNode.getComputerSystem() == null
                    || !composedNode.isEnabledAndHealthy()) {
                throw new IllegalStateException("Composed node should be enabled and healthy in order to invoke actions on it.");
            }

            bootSourceOverrideInvoker.overrideBootSource(composedNode.getComputerSystem(), type, state);
        } catch (ActionException e) {
            logger.d("Override boot resource action failed: error: {}, details: {}", e.getErrorResponse(), e.getMessage());
            throw new EntityOperationException("Conflict during overriding boot source: " + e.getMessage(), e);
        }
    }

    @Override
    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void deleteComposedNode(Context composedNodeContext) throws EntityNotFoundException, EntityOperationException {
        ComposedNode composedNode = composedNodeReader.getComposedNodeByContext(composedNodeContext);
        try {
            nodeDisassembler.disassemble(composedNode);
        } catch (DisassemblyException e) {
            logger.d("Disassembly failed: error: {}, details: {}", e.getErrorResponse(), e.getMessage());
            throw new EntityOperationException("Disassembly failed: " + e.getMessage(), e);
        }
        genericDao.remove(composedNode);
    }

    @Override
    public void terminateComposedNodeTasks(Context composedNodeContext) {
        nodeTasksCoordinator.removeAllTasks(composedNodeContext.getId());
    }
}
