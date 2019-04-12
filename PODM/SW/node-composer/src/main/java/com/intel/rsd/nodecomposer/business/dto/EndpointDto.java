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

package com.intel.rsd.nodecomposer.business.dto;

import com.fasterxml.jackson.annotation.JsonGetter;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.rsd.nodecomposer.business.dto.IpAddresses.IpV4AddressDto;
import com.intel.rsd.nodecomposer.business.dto.IpAddresses.IpV6AddressDto;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.DurableNameFormat;
import com.intel.rsd.nodecomposer.types.EntityRole;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;
import lombok.Setter;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "protocol", "pciId", "identifiers", "connectedEntities",
    "redundancies", "ipTransportDetails", "hostReservationMemoryBytes", "links", "oem"
})
@Setter
@Getter
public class EndpointDto extends RedfishDto {
    private final Collection<ConnectedEntityDto> connectedEntities = new HashSet<>();
    @JsonProperty("Redundancy")
    private final Collection<RedundancyDto> redundancies = new HashSet<>();
    @JsonProperty("IPTransportDetails")
    private final Collection<IpTransportDetailsDto> ipTransportDetails = new HashSet<>();
    private final Links links = new Links();
    private Status status;
    @JsonProperty("EndpointProtocol")
    private Protocol protocol;
    private PciIdDto pciId;
    private Collection<IdentifierDto> identifiers = new HashSet<>();
    private Integer hostReservationMemoryBytes;
    private Oem oem = new Oem();

    public EndpointDto() {
        super("#Endpoint.v1_1_0.Endpoint");
    }

    public void addIdentifier(IdentifierDto identifier) {
        this.identifiers.add(identifier);
    }

    public void addConnectedEntity(ConnectedEntityDto connectedEntityDto) {
        this.connectedEntities.add(connectedEntityDto);
    }

    @Setter
    @Getter
    @JsonPropertyOrder({"durableName", "durableNameFormat"})
    public static final class IdentifierDto {
        private String durableName;
        private DurableNameFormat durableNameFormat;
    }

    @Setter
    @Getter
    @JsonPropertyOrder({"entityRole", "pciFunctionNumber", "pciClassCode", "pciId", "entityLink",
        "identifiers"})
    public static final class ConnectedEntityDto {
        private EntityRole entityRole;
        private Integer pciFunctionNumber;
        private String pciClassCode;
        @JsonProperty("EntityPciId")
        private PciIdDto pciId;
        private Collection<IdentifierDto> identifiers = new HashSet<>();
        private ODataId entityLink;
    }

    @Setter
    @Getter
    @JsonPropertyOrder({"vendorId", "deviceId", "subsystemId", "subsystemVendorId"})
    public static final class PciIdDto {
        private String vendorId;
        private String deviceId;
        private String subsystemId;
        private String subsystemVendorId;
    }

    @Setter
    @Getter
    @JsonPropertyOrder({"transportProtocol", "ipv4Address", "ipv6Address", "port"})
    public static final class IpTransportDetailsDto {
        private Protocol transportProtocol;
        @JsonInclude(NON_NULL)
        @JsonProperty("IPv4Address")
        private IpV4AddressDto ipv4Address;
        @JsonInclude(NON_NULL)
        @JsonProperty("IPv6Address")
        private IpV6AddressDto ipv6Address;
        @JsonInclude(NON_NULL)
        private Integer port;
    }

    @Setter
    @Getter
    @JsonPropertyOrder({"@odata.type", "ports", "oem"})
    public final class Links {
        @JsonProperty("@odata.type")
        private final String oDataType = "#Endpoint.v1_1_0.Links";
        private Set<ODataId> ports = new HashSet<>();
        private final Oem oem = new Oem();

        public void addPort(ODataId oDataId) {
            ports.add(oDataId);
        }

        public Oem getOem() {
            return oem;
        }

        @JsonInclude(NON_NULL)
        public final class Oem {
            @JsonProperty("Intel_RackScale")
            private final RackScaleOem rackScaleOem = new RackScaleOem();

            @JsonIgnore
            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonGetter("Intel_RackScale")
            public RackScaleOem getRackScaleOemJson() {
                return checkRackScaleOemValues() ? rackScaleOem : null;
            }

            private boolean checkRackScaleOemValues() {
                return rackScaleOem.getZones() != null || rackScaleOem.getInterfaces() != null;
            }

            @Setter
            @Getter
            @JsonInclude(NON_NULL)
            @JsonPropertyOrder({"zones", "interfaces"})
            public final class RackScaleOem {
                private Set<ODataId> zones = new HashSet<>();
                private Set<ODataId> interfaces = new HashSet<>();
            }
        }
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @Setter
        @Getter
        @JsonPropertyOrder({"@odata.type", "authentication"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Endpoint";
            private Authentication authentication = new Authentication();

            @Setter
            @Getter
            @JsonPropertyOrder({"username", "password"})
            public class Authentication {
                private String username;
                private String password;
            }
        }
    }

}
