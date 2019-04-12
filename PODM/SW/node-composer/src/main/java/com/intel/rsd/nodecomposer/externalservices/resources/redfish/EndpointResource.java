/*
 * Copyright (c) 2016-2019 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Endpoint" + VERSION_PATTERN + "Endpoint"
})
public class EndpointResource extends ExternalServiceResourceImpl {
    @Getter
    @JsonProperty("Status")
    private Status status;

    @Getter
    @JsonProperty("EndpointProtocol")
    private Protocol protocol;

    @Getter
    @JsonProperty("Identifiers")
    private Set<IdentifierObject> identifiers;

    @JsonProperty("ConnectedEntities")
    private List<ConnectedEntityResource> connectedEntities = new ArrayList<>();

    @JsonProperty("IPTransportDetails")
    private List<IpTransportDetailsResource> transports = new ArrayList<>();

    @JsonProperty("Links")
    private Links links = new Links();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public AuthenticationObject getAuthentication() {
        return oem.rackScaleOem.authentication;
    }

    public Protocol getEndpointProtocolFromOem() {
        return oem.rackScaleOem.endpointProtocol;
    }

    @LinkName("connectedEntityInEndpoint")
    public Iterable<ResourceSupplier> getConnectedEntities() {
        return toSuppliersFromEmbeddableResourceElement(connectedEntities, "ConnectedEntities");
    }

    public List<ConnectedEntityResource> getConnectedEntityResourceList() {
        return connectedEntities;
    }

    @LinkName("transportInEndpoint")
    public Iterable<ResourceSupplier> getTransports() {
        return toSuppliersFromEmbeddableResourceElement(transports, "Transports");
    }

    public List<IpTransportDetailsResource> getTransportResourceList() {
        return transports;
    }

    @LinkName("portsInEndpoint")
    public Iterable<ResourceSupplier> getPorts() throws WebClientRequestException {
        return toSuppliers(links.ports);
    }

    @LinkName("ethernetInterfaceInEndpoint")
    public Iterable<ResourceSupplier> getInterfaces() throws WebClientRequestException {
        return toSuppliers(links.oem.rackScaleOem.interfaces);
    }

    public Set<ODataId> getZonesOdataIds() {
        return links.oem.rackScaleOem.zones;
    }

    public class Links {
        @JsonProperty("Ports")
        private Set<ODataId> ports = new HashSet<>();

        @JsonProperty("Oem")
        private Oem oem = new Oem();

        public final class Oem {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonInclude(NON_NULL)
            public final class RackScaleOem {
                @JsonProperty("Interfaces")
                private Set<ODataId> interfaces = new HashSet<>();

                @JsonProperty("Zones")
                private Set<ODataId> zones = new HashSet<>();
            }
        }
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("Authentication")
            private AuthenticationObject authentication = new AuthenticationObject();

            @JsonProperty("EndpointProtocol")
            private Protocol endpointProtocol;
        }
    }
}
