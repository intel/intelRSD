/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonInclude;
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
import com.intel.podm.common.types.redfish.OemType;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

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

    @JsonProperty("IPTransportDetails")
    private List<IpTransportDetailsResource> transports = new ArrayList<>();

    @JsonProperty("Redundancy")
    private Set<RedundancyItem> redundancies = new LinkedHashSet<>();

    @JsonProperty("PciId")
    private PciIdResource pciId;

    @JsonProperty("HostReservationMemoryBytes")
    private Integer hostReservationMemoryBytes;

    @JsonProperty("Links")
    private Links links = new Links();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

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

    public AuthenticationObject getAuthentication() {
        return oem.rackScaleOem.authentication;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("connectedEntityInEndpoint")
    public Iterable<ResourceSupplier> getConnectedEntities() throws WebClientRequestException {
        return toSuppliersFromEmbeddableResourceElement(connectedEntities, "ConnectedEntities");
    }

    public List<ConnectedEntityResource> getConnectedEntityResourceList() {
        return connectedEntities;
    }

    @LinkName("transportInEndpoint")
    public Iterable<ResourceSupplier> getTransports() throws WebClientRequestException {
        return toSuppliersFromEmbeddableResourceElement(transports, "Transports");
    }

    public List<IpTransportDetailsResource> getTransportResourceList() {
        return transports;
    }

    @LinkName("redundancy")
    public Iterable<ResourceSupplier> getRedundancies() throws WebClientRequestException {
        return toSuppliersFromResources(redundancies);
    }

    @LinkName("portsInEndpoint")
    public Iterable<ResourceSupplier> getPorts() throws WebClientRequestException {
        return toSuppliers(links.ports);
    }

    @LinkName("ethernetInterfaceInEndpoint")
    public Iterable<ResourceSupplier> getInterfaces() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.interfaces);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Ports")
        private Set<ODataId> ports = new HashSet<>();

        @JsonProperty("Oem")
        private Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public final class Oem extends RedfishOem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonInclude(NON_NULL)
            public final class RackScaleOem {
                @JsonProperty("Interfaces")
                private Set<ODataId> interfaces = new HashSet<>();
            }
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("Authentication")
            private AuthenticationObject authentication = new AuthenticationObject();
        }
    }
}
