/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.configuration;

import com.intel.rsd.java.lang.Pair;
import com.intel.rsd.nodecomposer.persistence.dao.ComposedNodeDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;
import java.util.function.Predicate;

import static com.intel.rsd.java.lang.Pair.pairOf;
import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingle;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Component
@SuppressWarnings("checkstyle:MethodCount")
public class UuidEndpointsMapProvider {
    private final InitiatorEndpointUuidExtractor uuidExtractor;
    private final GenericDao genericDao;
    private final ComposedNodeDao composedNodeDao;

    @Autowired
    public UuidEndpointsMapProvider(InitiatorEndpointUuidExtractor uuidExtractor, GenericDao genericDao, ComposedNodeDao composedNodeDao) {
        this.uuidExtractor = uuidExtractor;
        this.genericDao = genericDao;
        this.composedNodeDao = composedNodeDao;
    }

    @Transactional(MANDATORY)
    public Map<UUID, Collection<Endpoint>> createInitiatorUuidToZoneEndpointsMap(Protocol protocol) {
        Set<Fabric> fabrics = genericDao.findAll(Fabric.class).stream()
            .filter(fabric -> protocol.equals(fabric.getFabricType()))
            .collect(toSet());

        Map<UUID, Collection<Endpoint>> zoneInitiatorEndpointMapping = new HashMap<>();

        for (Fabric fabric : fabrics) {
            getNotEmptyZones(fabric).forEach(zone -> {
                Protocol fabricType = fabric.getFabricType();
                tryCreateUuidEndpointCollectionPair(fabricType, zone).ifPresent(pair -> {
                    Set<Endpoint> endpoints = getCompleteEndpointsCollection(zoneInitiatorEndpointMapping.get(pair.first()), pair);
                    zoneInitiatorEndpointMapping.put(pair.first(), endpoints);
                });
            });
        }

        return zoneInitiatorEndpointMapping;
    }

    private Collection<Zone> getNotEmptyZones(Fabric fabric) {
        return fabric.getZones().stream()
            .filter(zone -> !nullOrEmpty(zone.getEndpoints()))
            .collect(toSet());
    }

    private Set<Endpoint> getCompleteEndpointsCollection(Collection<Endpoint> existingEndpoints, Pair<UUID, Collection<Endpoint>> pair) {
        Set<Endpoint> expectedEndpoints = new HashSet<>(pair.second());
        if (existingEndpoints != null) {
            expectedEndpoints.addAll(existingEndpoints.stream()
                .filter(isTargetEndpoint())
                .collect(toSet()));
        }

        return expectedEndpoints;
    }

    private Optional<Pair<UUID, Collection<Endpoint>>> tryCreateUuidEndpointCollectionPair(Protocol fabricType, Zone zone) {
        switch (fabricType) {
            case NVME_OVER_FABRICS:
                return prepareMappingForNvme(zone);
            case FPGA_OVER_FABRICS:
                return prepareMappingForFpga(zone);
            default:
                return empty();
        }
    }

    private Optional<Pair<UUID, Collection<Endpoint>>> prepareMappingForFpga(Zone zone) {
        Set<Endpoint> zoneEndpoints = zone.getEndpoints();

        Optional<Endpoint> initiatorEndpointCandidate = zoneEndpoints.stream()
            .filter(endpoint -> endpoint.hasRole(INITIATOR))
            .filter(initiator -> tryFindComposedNodeRelatedWithEndpoint(initiator).isPresent())
            .findFirst();

        return initiatorEndpointCandidate.isPresent() ? tryCreateInitiatorUuidToEndpointMapping(initiatorEndpointCandidate.get(), zoneEndpoints) : empty();
    }

    private Optional<ComposedNode> tryFindComposedNodeRelatedWithEndpoint(Endpoint initiatorEndpoint) {
        UUID uuid = retrieveUuidFromInitiator(initiatorEndpoint);
        return composedNodeDao.tryFindComposedNodeByComputerSystemUuid(uuid);
    }

    private Optional<Pair<UUID, Collection<Endpoint>>> prepareMappingForNvme(Zone zone) {
        Set<Endpoint> zoneEndpoints = zone.getEndpoints();

        Endpoint initiatorEndpoint = zoneEndpoints.stream()
            .filter(endpoint -> endpoint.hasRole(INITIATOR))
            .collect(toSingle());

        return tryCreateInitiatorUuidToEndpointMapping(initiatorEndpoint, zoneEndpoints);
    }

    private Optional<Pair<UUID, Collection<Endpoint>>> tryCreateInitiatorUuidToEndpointMapping(Endpoint initiatorEndpoint,
                                                                                               Collection<Endpoint> allEndpointsInZone) {
        if (isValidZoneConfiguration(allEndpointsInZone)) {
            UUID uuid = retrieveUuidFromInitiator(initiatorEndpoint);
            return of(pairOf(uuid, allEndpointsInZone));
        }

        return empty();
    }

    private UUID retrieveUuidFromInitiator(Endpoint initiator) {
        return uuidExtractor.extractFromInitiatorEndpoint(initiator);
    }

    private boolean isValidZoneConfiguration(Collection<Endpoint> endpoints) {
        return endpoints.stream().anyMatch(isTargetEndpoint())
            && endpoints.stream().anyMatch(isInitiatorEndpoint());
    }

    private Predicate<Endpoint> isTargetEndpoint() {
        return endpoint -> endpoint.hasRole(TARGET);
    }

    private Predicate<Endpoint> isInitiatorEndpoint() {
        return endpoint -> endpoint.hasRole(INITIATOR);
    }
}
