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

package com.intel.podm.services.configuration.tasks;

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.redfish.EndpointResource;
import com.intel.podm.client.resources.redfish.FabricResource;
import com.intel.podm.client.resources.redfish.ServiceRootResource;
import com.intel.podm.client.resources.redfish.ZoneResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.configuration.difference.EndpointCollectionsDifferenceExtractor;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.UUID;
import java.util.function.Consumer;

import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.services.configuration.tasks.UpdateZoneTask.newUpdateZoneTaskBuilder;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.stream.Collectors.toList;

@Dependent
public class DiscoveryServiceTaskObtainer {
    @Inject
    private Logger logger;

    @Inject
    private EndpointCollectionsDifferenceExtractor differenceExtractor;

    public List<DiscoveryServiceTask> getListOfTasks(WebClient webClient, Map<UUID, Collection<Endpoint>> expectedEndpointsMap,
                                                     Map<UUID, ZoneResource> existingZonesMap) {

        URI fabricUri = getFabricResource(webClient).getUri();
        List<DiscoveryServiceTask> discoveryServiceTasks = new ArrayList<>();
        for (Entry<UUID, Collection<Endpoint>> entry : expectedEndpointsMap.entrySet()) {
            UUID uuid = entry.getKey();
            Collection<Endpoint> expectedZoneContent = entry.getValue();

            ZoneResource zoneResource = existingZonesMap.remove(uuid);
            if (zoneResource != null) {
                discoveryServiceTasks.addAll(getZoneUpdateTasks(zoneResource, expectedZoneContent, fabricUri));
            } else {
                discoveryServiceTasks.add(new CreateZoneTask(fabricUri, expectedZoneContent));
            }
        }

        discoveryServiceTasks.addAll(getDeleteZoneTasks(existingZonesMap.values()));

        return discoveryServiceTasks;
    }

    private FabricResource getFabricResource(WebClient webClient) {
        try {
            ServiceRootResource serviceRootResource = (ServiceRootResource) webClient.get(webClient.getBaseUri());
            return (FabricResource) single(serviceRootResource.getFabrics()).get();
        } catch (WebClientRequestException e) {
            String msg = format("Retrieving fabric uri failed on %s service", webClient.getBaseUri());
            logger.e(msg, e.getMessage());
            throw new RuntimeException(msg, e);
        }
    }

    private List<UpdateZoneTask> getZoneUpdateTasks(ZoneResource zoneResource, Collection<Endpoint> expectedEndpoints, URI fabricUri) {
        List<EndpointResource> existingEndpointResources = getEndpointResources(zoneResource);

        List<Endpoint> endpointsToAdd = differenceExtractor.getEndpointsToAdd(expectedEndpoints, existingEndpointResources);
        List<Endpoint> endpointsToRemove = differenceExtractor.getEndpointsToRemove(expectedEndpoints, existingEndpointResources);

        if (zoneNeedsToBeUpdated(endpointsToAdd, endpointsToRemove)) {
            return singletonList(newUpdateZoneTaskBuilder(fabricUri, zoneResource.getUri())
                .endpointsToCreate(endpointsToAdd)
                .endpointsToRemove(endpointsToRemove)
                .existingEndpoints(existingEndpointResources)
                .build());
        }

        return emptyList();
    }

    private boolean zoneNeedsToBeUpdated(List<Endpoint> endpointsToAdd, List<Endpoint> endpointsToRemove) {
        return !nullOrEmpty(endpointsToAdd) || !nullOrEmpty(endpointsToRemove);
    }

    private List<DiscoveryServiceTask> getDeleteZoneTasks(Collection<ZoneResource> zonesToRemove) {
        return zonesToRemove.stream()
            .map(zoneToRemoveResource -> new DeleteZoneTask(zoneToRemoveResource.getUri(), getEndpointResources(zoneToRemoveResource)))
            .collect(toList());
    }

    private List<EndpointResource> getEndpointResources(ZoneResource zoneResource) {
        List<EndpointResource> endpoints = new ArrayList<>();
        try {
            zoneResource.getEndpoints().forEach(getEndpointResourceFromResourceSupplier(endpoints));
        } catch (WebClientRequestException e) {
            logger.e(format("Exception while extracting endpoints from zone resource %s", e.getResourceUri()), e);
        }
        return endpoints;
    }

    private Consumer<ResourceSupplier> getEndpointResourceFromResourceSupplier(List<EndpointResource> endpoints) {
        return resourceSupplier -> {
            try {
                endpoints.add((EndpointResource) resourceSupplier.get());
            } catch (WebClientRequestException e) {
                logger.e(format("Exception while extracting EndpointResource from ResourceSupplier %s", e.getResourceUri()), e);
            }
        };
    }
}
