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

import com.google.common.base.MoreObjects;
import com.intel.rsd.nodecomposer.externalservices.actions.ZoneActionJson;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EndpointResource;
import lombok.AllArgsConstructor;

import java.net.URI;
import java.util.List;
import java.util.Set;

import static java.util.Collections.emptySet;
import static java.util.Collections.singleton;
import static java.util.stream.Collectors.toSet;

@AllArgsConstructor
public class DeleteZoneTask extends DiscoveryServiceTask {
    private final URI zoneToBeDeletedUri;
    private final List<EndpointResource> endpointsToBeRemoved;

    @Override
    public void perform() {
        Set<URI> endpointToRemoveUris = endpointsToBeRemoved.stream().map(ExternalServiceResourceImpl::getUri).collect(toSet());

        detachEndpointsFromZone(discoveryServiceUri, zoneToBeDeletedUri);
        deleteResources(discoveryServiceUri, endpointToRemoveUris);
        deleteResources(discoveryServiceUri, singleton(zoneToBeDeletedUri));
    }

    private void deleteResources(URI serviceUri, Set<URI> resourcesUris) {
        resourcesUris.forEach(resourceUri -> discoveryServiceWebClient.delete(serviceUri, resourceUri));
    }

    private void detachEndpointsFromZone(URI serviceBaseUri, URI zoneUri) {
        discoveryServiceWebClient.patch(serviceBaseUri, zoneUri, new ZoneActionJson(emptySet()));
    }


    @Override
    public String toString() {
        return MoreObjects.toStringHelper(this)
            .add("zoneToBeDeletedUri", zoneToBeDeletedUri)
            .add("endpointsToBeRemoved", endpointsToBeRemoved)
            .toString();
    }
}
