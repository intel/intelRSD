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
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.client.actions.ZoneActionJson;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.redfish.EndpointResource;

import java.net.URI;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.podm.services.configuration.client.EndpointCreationRequestBuilder.toEndpointCreationRequest;
import static java.net.URI.create;
import static java.util.stream.Collectors.toSet;
import static javax.ws.rs.core.UriBuilder.fromUri;

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

        restActionInvoker.patch(discoveryServiceUri, zoneResourceUri, new ZoneActionJson(expectedEndpoints));
        endpointsToRemove.forEach(endpointUri -> restActionInvoker.delete(discoveryServiceUri, endpointUri));
    }

    private Set<URI> createEndpoints() {
        URI endpointCollectionUri = fromUri(fabricUri).path(ENDPOINTS_RESOURCE_NAME).build();

        return endpointsToCreate.stream()
            .map(endpoint -> {
                URI createdEndpointUri = restActionInvoker.post(discoveryServiceUri, endpointCollectionUri, toEndpointCreationRequest(endpoint));
                return create(createdEndpointUri.getPath());
            }).collect(toSet());
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
                .map(DiscoverableEntity::getSourceUri)
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
