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

package com.intel.podm.services.configuration.client;

import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.FabricResource;
import com.intel.podm.client.resources.redfish.ServiceRootResource;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.configuration.ComputerSystemUuidExtractor;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.UUID;

import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static com.intel.podm.common.types.Pair.pairOf;
import static com.intel.podm.common.utils.IterableHelper.single;
import static java.lang.String.format;
import static java.util.Optional.empty;
import static java.util.Optional.of;

@Dependent
public class InitiatorFinder {
    @Inject
    private Logger logger;

    @Inject
    private ComputerSystemUuidExtractor uuidExtractor;

    public Map<UUID, ZoneResource> createComputerSystemUuidWithInitiatorEndpointMapping(WebClient webClient) {
        Map<UUID, ZoneResource> map = new HashMap<>();
        for (ZoneResource zoneResource : retrieveZonesFromDiscoveryService(webClient)) {
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

    private List<ZoneResource> retrieveZonesFromDiscoveryService(WebClient webClient) {
        List<ZoneResource> zoneResources = new ArrayList<>();
        try {
            ServiceRootResource serviceRoot = (ServiceRootResource) webClient.get(webClient.getBaseUri());
            FabricResource fabricResource = (FabricResource) single(serviceRoot.getFabrics()).get();

            for (ResourceSupplier resourceSupplier : fabricResource.getZones()) {
                zoneResources.add((ZoneResource) resourceSupplier.get());
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
            logger.e(format("Error while looking for initiator endpoint in Zone: %s", zoneResource.getUri()), e);
        }

        return empty();
    }

    private boolean isInitiatorEndpoint(EndpointResource endpointResource) {
        return endpointResource.getConnectedEntityResourceList().stream().anyMatch(connectedEntity -> INITIATOR.equals(connectedEntity.getEntityRole()));
    }
}
