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
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;

import java.net.URI;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.rsd.nodecomposer.services.configuration.client.EndpointCreationRequestBuilder.toEndpointCreationRequest;
import static com.intel.rsd.nodecomposer.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static java.net.URI.create;
import static java.util.stream.Collectors.toSet;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

public final class UpdateZoneTask extends DiscoveryServiceTask {
    private URI fabricUri;
    private URI zoneResourceUri;
    private List<Endpoint> endpointsToCreate;
    private Set<URI> endpointsToRemove;
    private Set<URI> existingEndpoints;

    private UpdateZoneTask(Builder builder) {
        this.fabricUri = builder.fabricUri;
        this.zoneResourceUri = builder.zoneResourceUri;
        this.endpointsToCreate = builder.endpointsToCreate;
        this.endpointsToRemove = builder.endpointsToRemove;
        this.existingEndpoints = builder.existingEndpointUris;
    }

    static Builder newUpdateZoneTaskBuilder(URI fabricUri, URI zoneResourceUri) {
        return new Builder(fabricUri, zoneResourceUri);
    }

    @Override
    public void perform() {
        Set<URI> expectedEndpoints = new HashSet<>(existingEndpoints);
        expectedEndpoints.removeAll(endpointsToRemove);

        expectedEndpoints.addAll(createEndpoints());

        discoveryServiceWebClient.patch(discoveryServiceUri, zoneResourceUri, new ZoneActionJson(expectedEndpoints));
        endpointsToRemove.forEach(endpointUri -> discoveryServiceWebClient.delete(discoveryServiceUri, endpointUri));
    }

    private Set<URI> createEndpoints() {
        URI endpointCollectionUri = fromUri(fabricUri).pathSegment(ENDPOINTS_RESOURCE_NAME).build().toUri();

        return endpointsToCreate.stream()
            .map(endpoint -> {
                URI createdEndpointUri = discoveryServiceWebClient.post(discoveryServiceUri, endpointCollectionUri, toEndpointCreationRequest(endpoint));
                return create(createdEndpointUri.getPath());
            }).collect(toSet());
    }

    @Override
    public String toString() {
        return MoreObjects.toStringHelper(this)
            .add("fabricUri", fabricUri)
            .add("zoneResourceUri", zoneResourceUri)
            .add("endpointsToCreate", endpointsToCreate)
            .add("endpointsToRemove", endpointsToRemove)
            .add("existingEndpoints", existingEndpoints)
            .toString();
    }

    public static final class Builder {
        private URI fabricUri;
        private URI zoneResourceUri;
        private List<Endpoint> endpointsToCreate;
        private Set<URI> endpointsToRemove;
        private Set<URI> existingEndpointUris;

        private Builder(URI fabricUri, URI zoneResourceUri) {
            this.fabricUri = fabricUri;
            this.zoneResourceUri = zoneResourceUri;
        }

        public Builder endpointsToCreate(List<Endpoint> endpointsToCreate) {
            this.endpointsToCreate = endpointsToCreate;
            return this;
        }

        public Builder endpointsToRemove(List<Endpoint> endpointsToRemove) {
            this.endpointsToRemove = endpointsToRemove.stream()
                .map(endpoint -> endpoint.getUri().toUri())
                .collect(toSet());
            return this;
        }

        public Builder existingEndpoints(List<EndpointResource> existingEndpointResources) {
            this.existingEndpointUris = existingEndpointResources.stream()
                .map(ExternalServiceResourceImpl::getUri)
                .collect(toSet());
            return this;
        }

        public UpdateZoneTask build() {
            return new UpdateZoneTask(this);
        }

    }
}
