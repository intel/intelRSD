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

package com.intel.rsd.nodecomposer.services.configuration.tasks;

import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.FabricResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.ZoneResource;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.services.configuration.difference.EndpointCollectionsDifferenceExtractor;
import lombok.NonNull;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Optional;
import java.util.UUID;
import java.util.concurrent.Callable;
import java.util.function.Consumer;

import static com.intel.rsd.nodecomposer.services.configuration.tasks.UpdateZoneTask.newUpdateZoneTaskBuilder;
import static com.intel.rsd.nodecomposer.utils.Collections.nullOrEmpty;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;

@Slf4j
@Component
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class DiscoveryServiceTaskObtainer {
    private final EndpointCollectionsDifferenceExtractor differenceExtractor;

    @Autowired
    public DiscoveryServiceTaskObtainer(EndpointCollectionsDifferenceExtractor differenceExtractor) {
        this.differenceExtractor = differenceExtractor;
    }

    public List<DiscoveryServiceTask> getListOfTasks(Map<UUID, Collection<Endpoint>> expectedEndpointsMap,
                                                     Map<UUID, ZoneResource> existingZonesMap,
                                                     @NonNull FabricResource fabricResource) {

        val tasks = new ArrayList<DiscoveryServiceTask>();
        tasks.addAll(prepareDeleteOrphanInitiatorEndpointTasks(fabricResource));
        tasks.addAll(prepareZoneReconfigurationTasks(expectedEndpointsMap, existingZonesMap, fabricResource));
        return tasks;
    }

    private Collection<DiscoveryServiceTask> prepareDeleteOrphanInitiatorEndpointTasks(FabricResource fabricResource) {
        return findEndpointsWithoutZone(fabricResource).stream().map(DeleteEndpointTask::new).collect(toList());
    }

    private Collection<DiscoveryServiceTask> prepareZoneReconfigurationTasks(Map<UUID, Collection<Endpoint>> expectedEndpointsMap,
                                                                             Map<UUID, ZoneResource> existingZonesMap,
                                                                             FabricResource fabricResource) {
        val result = new ArrayList<DiscoveryServiceTask>();
        for (Entry<UUID, Collection<Endpoint>> entry : expectedEndpointsMap.entrySet()) {
            val expectedZoneContent = entry.getValue();
            val zoneResource = existingZonesMap.remove(entry.getKey());
            if (zoneResource != null) {
                result.addAll(getZoneUpdateTasks(zoneResource, expectedZoneContent, fabricResource.getUri()));
            } else {
                result.add(new CreateZoneTask(fabricResource.getUri(), expectedZoneContent));
            }
        }

        result.addAll(getDeleteZoneTasks(existingZonesMap.values()));
        return result;
    }

    private List<EndpointResource> findEndpointsWithoutZone(FabricResource fabricResource) {
        val existingEndpoints = tryToGet(fabricResource::getEndpoints)
            .map(this::readEndpoints)
            .orElseGet(Collections::emptyList);

        return existingEndpoints.stream()
            .filter(endpointResource -> endpointResource.getZonesOdataIds().isEmpty())
            .collect(toList());
    }

    private List<EndpointResource> readEndpoints(Iterable<ResourceSupplier> resourceSuppliers) {
        return stream(resourceSuppliers.spliterator(), false)
            .map(endpointSupplier -> tryToGet(endpointSupplier::get))
            .filter(Optional::isPresent)
            .map(Optional::get)
            .map(EndpointResource.class::cast)
            .collect(toList());
    }

    @SuppressWarnings("checkstyle:IllegalCatch")
    private <T> Optional<T> tryToGet(Callable<T> callable) {
        try {
            return of(callable.call());
        } catch (Exception e) {
            log.error("Unexpected exception occurred during reading DS API", e);
            return empty();
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
            log.error(format("Exception while extracting endpoints from zone resource %s", e.getResourceUri()), e);
        }
        return endpoints;
    }

    private Consumer<ResourceSupplier> getEndpointResourceFromResourceSupplier(List<EndpointResource> endpoints) {
        return resourceSupplier -> {
            try {
                endpoints.add((EndpointResource) resourceSupplier.get());
            } catch (WebClientRequestException e) {
                log.error(format("Exception while extracting EndpointResource from ResourceSupplier %s", e.getResourceUri()), e);
            }
        };
    }
}
