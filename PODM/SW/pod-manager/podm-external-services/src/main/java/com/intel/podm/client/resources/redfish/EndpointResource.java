/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Endpoint" + VERSION_PATTERN + "Endpoint"
})
public class EndpointResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("EndpointProtocol")
    private Protocol protocol;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("ConnectedEntities")
    private List<ConnectedEntityResource> connectedEntities = new ArrayList<>();

    @JsonProperty("Identifiers")
    private Set<IdentifierObject> identifiers;

    @JsonProperty("Redundancy")
    private Set<RedundancyItem> redundancies = new LinkedHashSet<>();

    @JsonProperty("PciId")
    private PciIdResource pciId;

    @JsonProperty("HostReservationMemoryBytes")
    private Integer hostReservationMemoryBytes;

    @JsonProperty("Links")
    private Links links = new Links();

    public Protocol getProtocol() {
        return protocol;
    }

    public Integer getHostReservationMemoryBytes() {
        return hostReservationMemoryBytes;
    }

    public PciIdResource getPciId() {
        return pciId;
    }

    public Set<IdentifierObject> getIdentifiers() {
        return identifiers;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("connectedEntityInEndpoint")
    public Iterable<ResourceSupplier> getConnectedEntities() throws WebClientRequestException {
        return toSuppliersFromEmbeddableResourceElement(connectedEntities, "ConnectedEntities");
    }

    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancies() throws WebClientRequestException {
        return toSuppliersFromResources(redundancies);
    }

    @LinkName("portsInEndpoint")
    public Iterable<ResourceSupplier> getPorts() throws WebClientRequestException {
        return toSuppliers(links.ports);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Ports")
        private Set<ODataId> ports = new HashSet<>();
    }
}
