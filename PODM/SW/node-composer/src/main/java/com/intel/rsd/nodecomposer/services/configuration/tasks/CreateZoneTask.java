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
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import lombok.AllArgsConstructor;

import java.net.URI;
import java.util.Collection;
import java.util.Set;

import static com.intel.rsd.nodecomposer.services.configuration.client.EndpointCreationRequestBuilder.toEndpointCreationRequest;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ZONES_RESOURCE_NAME;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toSet;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@AllArgsConstructor
public class CreateZoneTask extends DiscoveryServiceTask {
    private final URI fabricUri;
    private final Collection<Endpoint> endpoints;

    @Override
    public void perform() {
        requiresNonNull(endpoints, "Endpoints");
        requiresNonNull(fabricUri, "FabricUri");

        Set<URI> createdEndpoints = createEndpoints(endpoints, fabricUri);
        URI zoneCollectionUri = fromUri(fabricUri).pathSegment(ZONES_RESOURCE_NAME).build().toUri();
        discoveryServiceWebClient.post(discoveryServiceUri, zoneCollectionUri, new ZoneActionJson(createdEndpoints));
    }

    private Set<URI> createEndpoints(Collection<Endpoint> endpoints, URI fabricUri) {
        URI endpointCollectionUri = fromUri(fabricUri).pathSegment(ENDPOINTS_RESOURCE_NAME).build().toUri();
        return endpoints.stream()
            .map(endpoint -> {
                URI createdEndpointUri = discoveryServiceWebClient.post(discoveryServiceUri, endpointCollectionUri, toEndpointCreationRequest(endpoint));
                return URI.create(createdEndpointUri.getPath());
            })
            .collect(toSet());
    }

    @Override
    public String toString() {
        return MoreObjects.toStringHelper(this)
            .add("fabricUri", fabricUri)
            .add("endpoints", endpoints)
            .toString();
    }
}
