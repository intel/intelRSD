/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.redfish.services.actions.EndpointUpdateInvoker;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.types.Chap;
import com.intel.podm.common.types.actions.EndpointUpdateDefinition;
import com.intel.podm.common.types.actions.EndpointUpdateDefinition.Authentication;
import com.intel.podm.discovery.external.partial.EndpointObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PatchEndpointsAssemblyTask extends NodeTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    private EndpointObtainer endpointObtainer;

    @Inject
    private EndpointUpdateInvoker endpointUpdateInvoker;

    private Chap chap;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        ComputerSystem computerSystem = getComputerSystemFromNode(composedNode);
        ConnectedEntity connectedEntity = findConnectedEntityWithBootableVolume(composedNode);
        Endpoint targetEndpoint = findTargetEndpoint(connectedEntity);
        Endpoint initiatorEndpoint = endpointObtainer.getInitiatorEndpoint(computerSystem, targetEndpoint.getFabric());

        try {
            updateInitiatorEndpoint(initiatorEndpoint);
            updateTargetEndpoint(targetEndpoint);
        } catch (EntityOperationException e) {
            throw new RuntimeException("Endpoint update failed: " + e.getMessage(), e);
        }
    }

    public void setChap(Chap chap) {
        this.chap = chap;
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        ConnectedEntity connectedEntity = findConnectedEntityWithBootableVolume(composedNode);
        Endpoint targetEndpoint = findTargetEndpoint(connectedEntity);

        return targetEndpoint.getService().getUuid();
    }

    private void updateInitiatorEndpoint(Endpoint endpoint) throws EntityOperationException {
        EndpointUpdateDefinition initiatorDefinition = new EndpointUpdateDefinition();
        Authentication authentication = initiatorDefinition.getOem().getRackScaleOem().getAuthentication();

        authentication.setUsername(chap.getUsername().get());
        authentication.setPassword(chap.getSecret().get());

        endpointUpdateInvoker.updateEndpoint(endpoint, initiatorDefinition);
    }

    private void updateTargetEndpoint(Endpoint endpoint) throws EntityOperationException {
        EndpointUpdateDefinition targetDefinition = new EndpointUpdateDefinition();
        Authentication authentication = targetDefinition.getOem().getRackScaleOem().getAuthentication();

        authentication.setUsername(chap.getMutualUsername().get());
        authentication.setPassword(chap.getMutualSecret().get());

        endpointUpdateInvoker.updateEndpoint(endpoint, targetDefinition);
    }

    private Endpoint findTargetEndpoint(ConnectedEntity connectedEntity) {
        return ofNullable(connectedEntity.getEndpoint())
            .orElseThrow(() -> new RuntimeException("Connection between bootable volume and target endpoint doesn't exist"));
    }

    private ConnectedEntity findConnectedEntityWithBootableVolume(ComposedNode composedNode) {
        return composedNode.findAnyConnectedEntityWithBootableVolume()
            .orElseThrow(() -> new RuntimeException("Composed node is not connected with any bootable volume"));
    }
}
