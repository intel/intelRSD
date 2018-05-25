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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.EndpointDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static java.util.Optional.empty;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class EndpointLinker {
    @Inject
    private ComputerSystemDao computerSystemDao;

    @Inject
    private EndpointDao endpointDao;

    @TimeMeasured(tag = "[Finalizer]")
    @Transactional(MANDATORY)
    void linkSystemToRelatedEndpoint(Collection<ComputerSystem> discoveredComputerSystems) {
        discoveredComputerSystems.forEach(this::addEndpointToSystem);
    }

    @TimeMeasured(tag = "[Finalizer]")
    @Transactional(MANDATORY)
    void linkEndpointToRelatedSystem(Collection<Endpoint> discoveredEndpoints) {
        for (Endpoint endpoint : discoveredEndpoints) {
            getInitiatorConnectedEntities(endpoint)
                .forEach(connectedEntity -> addSystemToEndpoint(connectedEntity, endpoint));
        }
    }

    private void addEndpointToSystem(ComputerSystem system) {
        for (Endpoint endpoint : endpointDao.findEndpointMatchingUuid(system.getUuid())) {
            getInitiatorConnectedEntities(endpoint)
                .forEach(connectedEntity -> link(connectedEntity, system));
        }
    }

    private void addSystemToEndpoint(ConnectedEntity connectedEntity, Endpoint endpoint) {
        Optional<ComputerSystem> systemRelatedWithEndpoint = tryGetRelatedSystem(endpoint.getIdentifiers());

        if (systemRelatedWithEndpoint.isPresent()) {
            ComputerSystem system = systemRelatedWithEndpoint.get();
            link(connectedEntity, system);
            ifNecessaryUpdateComposeNode(system, endpoint);
        }
    }

    private Optional<ComputerSystem> tryGetRelatedSystem(Set<Identifier> identifiers) {
        return identifiers.stream()
            .map(IdentifierUtils::tryGetUuidFromIdentifier)
            .filter(Optional::isPresent)
            .map(uuid -> tryGetComputerSystemBuUuid(uuid.get()))
            .findFirst()
            .orElse(empty());
    }

    private Optional<ComputerSystem> tryGetComputerSystemBuUuid(UUID uuid) {
        return computerSystemDao.getUniquePhysicalComputerSystemByUuid(uuid);
    }

    private void ifNecessaryUpdateComposeNode(ComputerSystem system, Endpoint endpoint) {
        ComposedNode composedNode = system.getComposedNode();
        if (composedNodeShouldBeUpdated(composedNode)) {
            Zone endpointZone = endpoint.getZone();
            if (endpointZone != null) {
                attachEndpoints(endpointZone, composedNode);
            }
        }
    }

    private boolean composedNodeShouldBeUpdated(ComposedNode composedNode) {
        return composedNode != null
            && composedNode.isEnabledAndHealthy()
            && composedNode.isInAnyOfStates(ASSEMBLED);
    }

    private List<ConnectedEntity> getInitiatorConnectedEntities(Endpoint endpoint) {
        return endpoint.getConnectedEntities().stream()
            .filter(connectedEntity -> INITIATOR.equals(connectedEntity.getEntityRole()))
            .collect(toList());
    }


    private void link(ConnectedEntity initiatorConnectedEntity, ComputerSystem computerSystem) {
        computerSystem.addEndpoint(initiatorConnectedEntity.getEndpoint());
        initiatorConnectedEntity.setEntityLink(computerSystem);
    }

    private void attachEndpoints(Zone zone, ComposedNode composedNode) {
        zone.getEndpoints().stream()
            .filter(endpoint -> getInitiatorConnectedEntities(endpoint).size() == 0)
            .filter(endpoint -> !composedNode.getEndpoints().contains(endpoint))
            .forEach(composedNode::addEndpoint);
    }
}
