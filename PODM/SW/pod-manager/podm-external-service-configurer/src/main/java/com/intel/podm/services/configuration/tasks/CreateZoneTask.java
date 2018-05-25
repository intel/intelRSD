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
import com.intel.podm.client.actions.ZoneActionJson;

import java.net.URI;
import java.util.Collection;
import java.util.Set;

import static com.intel.podm.common.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ZONES_RESOURCE_NAME;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static com.intel.podm.services.configuration.client.EndpointCreationRequestBuilder.toEndpointCreationRequest;
import static java.util.stream.Collectors.toSet;
import static javax.ws.rs.core.UriBuilder.fromUri;

public class CreateZoneTask extends DiscoveryServiceTask {
    private final URI fabricUri;
    private final Collection<Endpoint> endpoints;

    CreateZoneTask(URI fabricUri, Collection<Endpoint> endpoints) {
        this.fabricUri = fabricUri;
        this.endpoints = endpoints;
    }

    @Override
    public void perform() {
        requiresNonNull(endpoints, "Endpoints");
        requiresNonNull(fabricUri, "FabricUri");

        Set<URI> createdEndpoints = createEndpoints(endpoints, fabricUri);
        URI zoneCollectionUri = fromUri(fabricUri).path(ZONES_RESOURCE_NAME).build();
        restActionInvoker.post(discoveryServiceUri, zoneCollectionUri, new ZoneActionJson(createdEndpoints));
    }

    private Set<URI> createEndpoints(Collection<Endpoint> endpoints, URI fabricUri) {
        URI endpointCollectionUri = fromUri(fabricUri).path(ENDPOINTS_RESOURCE_NAME).build();
        return endpoints.stream()
            .map(endpoint -> {
                URI createdEndpointUri = restActionInvoker.post(discoveryServiceUri, endpointCollectionUri, toEndpointCreationRequest(endpoint));
                return URI.create(createdEndpointUri.getPath());
            })
            .collect(toSet());
    }
}
