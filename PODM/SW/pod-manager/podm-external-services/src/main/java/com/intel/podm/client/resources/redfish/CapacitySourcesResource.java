/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceLinks;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ODataId;

import java.net.URI;
import java.util.HashSet;
import java.util.Set;

@OdataTypes({
    "Capacity\\.CapacitySource"
})
public class CapacitySourcesResource implements ExternalServiceResource {
    @JsonIgnore
    private WebClient webClient;

    @JsonIgnore
    private URI uri;

    @JsonProperty("ProvidedCapacity")
    private CapacityResource providedCapacity;

    @JsonProperty("ProvidingVolumes")
    private Set<ODataId> providingVolumes = new HashSet<>();

    @JsonProperty("ProvidingPools")
    private Set<ODataId> providingPools = new HashSet<>();

    @JsonProperty("ProvidingDrives")
    private Set<ODataId> providingDrives = new HashSet<>();

    public CapacityResource getProvidedCapacity() {
        return providedCapacity;
    }

    @LinkName("providingDrives")
    public Iterable<ResourceSupplier> getProvidingDrives() throws WebClientRequestException {
        return toSuppliers(webClient, providingDrives);
    }

    public Set<ODataId> getProvidingVolumes() {
        return providingVolumes;
    }

    @LinkName("providingPools")
    public Iterable<ResourceSupplier> getProvidingPools() throws WebClientRequestException {
        return toSuppliers(webClient, providingPools);
    }

    @Override
    public URI getUri() {
        return uri;
    }

    @Override
    public void setUri(URI uri) {
        this.uri = uri;
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }

    @Override
    public void setWebClient(WebClient webClient) {
        this.webClient = webClient;
    }
}
