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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.EndpointResource;
import com.intel.podm.client.api.resources.redfish.IdentifierObject;
import com.intel.podm.client.api.resources.redfish.PciIdResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.client.resources.redfish.properties.IdentifierObjectImpl;
import com.intel.podm.client.resources.redfish.properties.PciIdResourceImpl;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

@OdataTypes({
    "#Endpoint" + OdataTypes.VERSION_PATTERN + "Endpoint"
})
public class EndpointResourceImpl extends ExternalServiceResourceImpl implements EndpointResource {
    @JsonProperty("EndpointProtocol")
    private Protocol protocol;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("ConnectedEntities")
    private List<ConnectedEntityResourceImpl> connectedEntities = new ArrayList<>();

    @JsonProperty("Identifiers")
    private Set<IdentifierObjectImpl> identifiers;

    @JsonProperty("Redundancy")
    private Set<RedundancyObjectImpl> redundancies = new LinkedHashSet<>();

    @JsonProperty("PciId")
    private PciIdResourceImpl pciId;

    @JsonProperty("HostReservationMemoryBytes")
    private Integer hostReservationMemoryBytes;

    @JsonProperty("Links")
    private Links links = new Links();

    @Override
    public Protocol getProtocol() {
        return protocol;
    }

    @Override
    public Integer getHostReservationMemoryBytes() {
        return hostReservationMemoryBytes;
    }

    @Override
    public PciIdResource getPciId() {
        return pciId;
    }

    @Override
    public Set<IdentifierObject> getIdentifiers() {
        return (Set) identifiers;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    @LinkName("connectedEntityInEndpoint")
    public Iterable<ResourceSupplier> getConnectedEntities() throws ExternalServiceApiReaderException {
        return toSuppliersFromEmbedabbleResourceElement(connectedEntities, "ConnectedEntities");
    }

    @Override
    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancies() throws ExternalServiceApiReaderException {
        return toSuppliersFromResources(redundancies);
    }

    @Override
    @LinkName("portsInEndpoint")
    public Iterable<ResourceSupplier> getPorts() throws ExternalServiceApiReaderException {
        return toSuppliers(links.ports);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Ports")
        private Set<ODataId> ports = new HashSet<>();
    }
}
