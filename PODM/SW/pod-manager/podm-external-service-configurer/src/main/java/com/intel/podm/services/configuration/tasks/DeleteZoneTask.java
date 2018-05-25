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

import com.intel.podm.client.actions.ZoneActionJson;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.redfish.EndpointResource;

import java.net.URI;
import java.util.List;
import java.util.Set;

import static java.util.Collections.emptySet;
import static java.util.Collections.singleton;
import static java.util.stream.Collectors.toSet;

public class DeleteZoneTask extends DiscoveryServiceTask {
    private final URI zoneToBeDeletedUri;
    private final List<EndpointResource> endpointsToBeRemoved;

    DeleteZoneTask(URI zoneToBeDeletedUri, List<EndpointResource> endpointsToBeRemoved) {
        this.zoneToBeDeletedUri = zoneToBeDeletedUri;
        this.endpointsToBeRemoved = endpointsToBeRemoved;
    }

    @Override
    public void perform() {
        Set<URI> endpointToRemoveUris = endpointsToBeRemoved.stream().map(ExternalServiceResourceImpl::getUri).collect(toSet());

        detachEndpointsFromZone(discoveryServiceUri, zoneToBeDeletedUri);
        deleteResources(discoveryServiceUri, endpointToRemoveUris);
        deleteResources(discoveryServiceUri, singleton(zoneToBeDeletedUri));
    }

    private void deleteResources(URI serviceUri, Set<URI> resourcesUris) {
        resourcesUris.forEach(resourceUri -> restActionInvoker.delete(serviceUri, resourceUri));
    }

    private void detachEndpointsFromZone(URI serviceBaseUri, URI zoneUri) {
        restActionInvoker.patch(serviceBaseUri, zoneUri, new ZoneActionJson(emptySet()));
    }
}
