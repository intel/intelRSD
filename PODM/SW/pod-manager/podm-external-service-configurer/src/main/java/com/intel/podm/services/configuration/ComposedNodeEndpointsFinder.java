/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.services.configuration;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.common.types.Pair;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static com.intel.podm.common.types.Pair.pairOf;
import static com.intel.podm.common.types.Protocol.NVME_OVER_FABRICS;
import static com.intel.podm.common.utils.IterableHelper.any;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.util.Collections.emptySet;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toMap;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class ComposedNodeEndpointsFinder {
    @Inject
    private ComputerSystemUuidExtractor uuidExtractor;

    @Inject
    private GenericDao genericDao;

    @Transactional(MANDATORY)
    public Map<UUID, Collection<Endpoint>> createComputerSystemUuidWithEndpointCollectionMapping() {
        List<ComposedNode> composedNodes = genericDao.findAll(ComposedNode.class);

        return getAllInitiatorEndpointCollectionsFromNodes(composedNodes).stream()
            .filter(collection -> !collection.isEmpty())
            .map(this::tryCreateUuidEndpointMapping)
            .filter(Optional::isPresent)
            .map(Optional::get)
            .collect(toMap(Pair::first, Pair::second));
    }

    private Optional<Pair<UUID, Collection<Endpoint>>> tryCreateUuidEndpointMapping(Set<Endpoint> initiatorEndpoints) {
        Collection<Endpoint> allEndpointsInZone = createZoneEndpointCollection(initiatorEndpoints);

        if (!allEndpointsInZone.isEmpty()) {
            UUID uuid = retrieveUuidFromInitiators(initiatorEndpoints);
            return of(pairOf(uuid, allEndpointsInZone));
        }

        return empty();
    }

    private Set<Set<Endpoint>> getAllInitiatorEndpointCollectionsFromNodes(List<ComposedNode> composedNodes) {
        return composedNodes.stream()
            .map(ComposedNode::getComputerSystem)
            .map(ComputerSystem::getEndpoints)
            .map(endpoints -> endpoints.stream()
                .filter(endpoint -> endpoint.hasRole(INITIATOR) && endpoint.getZone() != null)
                .filter(endpoint -> NVME_OVER_FABRICS.equals(endpoint.getFabric().getFabricType()))
                .collect(toSet()))
            .collect(toSet());
    }

    private Collection<Endpoint> createZoneEndpointCollection(Set<Endpoint> initiators) {
        Collection<Endpoint> endpoints = new HashSet<>();
        switch (initiators.size()) {
            case 0:
                break;
            case 1:
                Endpoint endpoint = single(initiators);
                Set<Endpoint> zoneEndpoints = endpoint.getZone().getEndpoints();
                endpoints.addAll(getValidZoneEndpoints(zoneEndpoints));
                break;
            default:
                Collection<Endpoint> targetEndpoints = findTargetEndpoints(initiators);
                targetEndpoints.add(any(initiators));
                endpoints.addAll(getValidZoneEndpoints(targetEndpoints));
        }

        return endpoints;
    }

    private Collection<Endpoint> getValidZoneEndpoints(Collection<Endpoint> endpoints) {
        if (isValidZoneConfiguration(endpoints)) {
            return endpoints;
        }

        return emptySet();
    }

    private UUID retrieveUuidFromInitiators(Collection<Endpoint> initiators) {
        // All Initiators within one ComputerSystem has the same NQN (containing UUID of ComputerSystem) thus the first one is taken to
        // newly created Zone.
        return uuidExtractor.extractFromInitiatorEndpoint(any(initiators));
    }

    private Collection<Endpoint> findTargetEndpoints(Set<Endpoint> initiators) {
        return initiators.stream()
            .map(Endpoint::getZone)
            .flatMap(zone -> zone.getEndpoints().stream())
            .filter(endpoint -> endpoint.hasRole(TARGET))
            .collect(toSet());
    }

    private boolean isValidZoneConfiguration(Collection<Endpoint> endpoints) {
        return endpoints.stream().anyMatch(endpoint -> endpoint.hasRole(TARGET))
            && endpoints.stream().anyMatch(endpoint -> endpoint.hasRole(INITIATOR));
    }
}
