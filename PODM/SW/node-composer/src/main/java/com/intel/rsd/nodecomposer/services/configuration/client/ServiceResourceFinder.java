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

package com.intel.rsd.nodecomposer.services.configuration.client;

import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.FabricResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ServiceRootResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ZoneResource;
import com.intel.rsd.nodecomposer.services.configuration.InitiatorEndpointUuidExtractor;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.UUID;

import static com.intel.rsd.java.lang.Pair.pairOf;
import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.types.Protocol.OEM;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.of;

@Slf4j
@Component
public class ServiceResourceFinder {
    private final InitiatorEndpointUuidExtractor uuidExtractor;

    @Autowired
    public ServiceResourceFinder(InitiatorEndpointUuidExtractor uuidExtractor) {
        this.uuidExtractor = uuidExtractor;
    }

    public Map<UUID, ZoneResource> createUuidWithInitiatorEndpointMapping(WebClient webClient, Protocol fabricProtocol) {
        Map<UUID, ZoneResource> map = new HashMap<>();
        for (ZoneResource zoneResource : retrieveZonesFromDiscoveryService(webClient, fabricProtocol)) {
            tryGetInitiatorEndpointResource(zoneResource)
                .map(initiator -> pairOf(zoneResource, initiator))
                .ifPresent(zoneEndpointPair -> {
                    ZoneResource initiatorZone = zoneEndpointPair.first();
                    EndpointResource initiatorEndpoint = zoneEndpointPair.second();
                    if (map.put(uuidExtractor.extractFromInitiatorEndpointResource(initiatorEndpoint), initiatorZone) != null) {
                        throw new IllegalStateException("Duplicate initiator endpoints found in zone!");
                    }
                });
        }
        return map;
    }

    public Optional<FabricResource> tryFindFabricResource(WebClient webClient, @NonNull Protocol protocol) {
        try {
            ServiceRootResource serviceRoot = (ServiceRootResource) webClient.get(webClient.getBaseUri());
            for (ResourceSupplier resourceSupplierFabric : serviceRoot.getFabrics()) {
                FabricResource fabricResource = (FabricResource) resourceSupplierFabric.get();
                if (protocol.equals(getFabricTypeFromResource(fabricResource))) {
                    return of(fabricResource);
                }
            }
            return empty();
        } catch (WebClientRequestException e) {
            String msg = format("Retrieving fabric uri failed on %s service", webClient.getBaseUri());
            log.error(msg, e.getMessage());
            throw new RuntimeException(msg, e);
        }
    }

    private Protocol getFabricTypeFromResource(FabricResource fabric) {
        if (OEM.equals(fabric.getFabricType())) {
            return fabric.getFabricTypeFromOem();
        }
        return fabric.getFabricType();
    }

    private List<ZoneResource> retrieveZonesFromDiscoveryService(WebClient webClient, @NonNull Protocol fabricProtocol) {
        List<ZoneResource> zoneResources = new ArrayList<>();
        try {
            Optional<FabricResource> fabric = tryFindFabricResource(webClient, fabricProtocol);
            if (fabric.isPresent()) {
                for (ResourceSupplier resourceSupplier : fabric.get().getZones()) {
                    zoneResources.add((ZoneResource) resourceSupplier.get());
                }
            }
        } catch (WebClientRequestException e) {
            throw new RuntimeException("Could not obtain Zones from DiscoveryService", e);
        }
        return zoneResources;
    }

    private Optional<EndpointResource> tryGetInitiatorEndpointResource(ZoneResource zoneResource) {
        try {
            for (ResourceSupplier resourceSupplier : zoneResource.getEndpoints()) {
                EndpointResource endpointResource = (EndpointResource) resourceSupplier.get();
                if (isInitiatorEndpoint(endpointResource)) {
                    return of(endpointResource);
                }
            }
        } catch (WebClientRequestException e) {
            log.error(format("Error while looking for initiator endpoint in Zone: %s", zoneResource.getUri()), e);
        }

        return empty();
    }

    private boolean isInitiatorEndpoint(EndpointResource endpointResource) {
        return endpointResource.getConnectedEntityResourceList().stream().anyMatch(connectedEntity -> INITIATOR.equals(connectedEntity.getEntityRole()));
    }
}
