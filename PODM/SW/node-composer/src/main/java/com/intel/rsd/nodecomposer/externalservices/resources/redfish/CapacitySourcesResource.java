/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceLinks;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import lombok.Getter;
import lombok.Setter;

import java.net.URI;
import java.util.HashSet;
import java.util.Set;

@OdataTypes({
    "Capacity\\.CapacitySource"
})
public class CapacitySourcesResource implements ExternalServiceResource {
    @Setter
    @JsonIgnore
    private WebClient webClient;

    @Getter
    @Setter
    @JsonIgnore
    private URI uri;

    @JsonProperty("ProvidingPools")
    private Set<ODataId> providingPools = new HashSet<>();

    @LinkName("providingPools")
    public Iterable<ResourceSupplier> getProvidingPools() throws WebClientRequestException {
        return toSuppliers(webClient, providingPools);
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }
}
