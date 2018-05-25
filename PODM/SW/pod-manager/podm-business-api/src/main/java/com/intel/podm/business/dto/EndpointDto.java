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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonGetter;
import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.IpAddresses.IpV4AddressDto;
import com.intel.podm.business.dto.IpAddresses.IpV6AddressDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.DurableIdentifier;
import com.intel.podm.common.types.redfish.OemType;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.business.services.context.UriToContextConverter.getContextFromUri;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "protocol", "pciId", "identifiers", "connectedEntities",
    "redundancies", "ipTransportDetails", "hostReservationMemoryBytes", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class EndpointDto extends RedfishDto {
    private final Collection<ConnectedEntityDto> connectedEntities = new HashSet<>();
    @JsonProperty("Redundancy")
    private final Collection<RedundancyDto> redundancies = new HashSet<>();
    @JsonProperty("IPTransportDetails")
    private final Collection<IpTransportDetailsDto> ipTransportDetails = new HashSet<>();
    private final Actions actions = new Actions();
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

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Protocol getProtocol() {
        return protocol;
    }

    public void setProtocol(Protocol protocol) {
        this.protocol = protocol;
    }

    public PciIdDto getPciId() {
        return pciId;
    }

    public void setPciId(PciIdDto pciId) {
        this.pciId = pciId;
    }

    public Collection<IdentifierDto> getIdentifiers() {
        return identifiers;
    }

    public void setIdentifiers(Collection<IdentifierDto> identifiers) {
        this.identifiers = identifiers;
    }

    public void addIdentifier(IdentifierDto identifier) {
        this.identifiers.add(identifier);
    }

    public Collection<ConnectedEntityDto> getConnectedEntities() {
        return connectedEntities;
    }

    public void addConnectedEntity(ConnectedEntityDto connectedEntityDto) {
        this.connectedEntities.add(connectedEntityDto);
    }

    public Collection<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public Collection<IpTransportDetailsDto> getIpTransportDetails() {
        return ipTransportDetails;
    }

    public void setIpTransportDetails(Collection<IpTransportDetailsDto> transports) {
        this.ipTransportDetails.clear();
        this.ipTransportDetails.addAll(transports);
    }

    public Integer getHostReservationMemoryBytes() {
        return hostReservationMemoryBytes;
    }

    public void setHostReservationMemoryBytes(Integer hostReservationMemoryBytes) {
        this.hostReservationMemoryBytes = hostReservationMemoryBytes;
    }

    public Actions getActions() {
        return actions;
    }

    public Links getLinks() {
        return links;
    }

    public Oem getOem() {
        return oem;
    }

    @JsonPropertyOrder({"durableName", "durableNameFormat"})
    public static final class IdentifierDto implements DurableIdentifier {
        private String durableName;
        private DurableNameFormat durableNameFormat;

        @Override
        public String getDurableName() {
            return durableName;
        }

        public void setDurableName(String durableName) {
            this.durableName = durableName;
        }

        @Override
        public DurableNameFormat getDurableNameFormat() {
            return durableNameFormat;
        }

        public void setDurableNameFormat(DurableNameFormat durableNameFormat) {
            this.durableNameFormat = durableNameFormat;
        }
    }

    @JsonPropertyOrder({"entityRole", "pciFunctionNumber", "pciClassCode", "pciId", "entityLink",
        "identifiers"})
    public static final class ConnectedEntityDto {
        private EntityRole entityRole;
        private Integer pciFunctionNumber;
        private String pciClassCode;
        @JsonProperty("EntityPciId")
        private PciIdDto pciId;
        private Collection<IdentifierDto> identifiers = new HashSet<>();
        private Context entityLink;

        @JsonProperty("EntityLink")
        public void setEntityLink(ODataId oDataId) {
            if (oDataId == null || oDataId.toUri() == null) {
                return;
            }
            this.entityLink = getContextFromUri(oDataId.toUri());
        }

        public EntityRole getEntityRole() {
            return entityRole;
        }

        public void setEntityRole(EntityRole entityRole) {
            this.entityRole = entityRole;
        }

        public Integer getPciFunctionNumber() {
            return pciFunctionNumber;
        }

        public void setPciFunctionNumber(Integer pciFunctionNumber) {
            this.pciFunctionNumber = pciFunctionNumber;
        }

        public String getPciClassCode() {
            return pciClassCode;
        }

        public void setPciClassCode(String pciClassCode) {
            this.pciClassCode = pciClassCode;
        }

        public PciIdDto getPciId() {
            return pciId;
        }

        public void setPciId(PciIdDto pciId) {
            this.pciId = pciId;
        }

        public Collection<IdentifierDto> getIdentifiers() {
            return identifiers;
        }

        public void setIdentifiers(Collection<IdentifierDto> identifiers) {
            this.identifiers = identifiers;
        }

        public Context getEntityLink() {
            return entityLink;
        }

        public void setEntityLink(Context entityLink) {
            this.entityLink = entityLink;
        }
    }

    @JsonPropertyOrder({"vendorId", "deviceId", "subsystemId", "subsystemVendorId"})
    public static final class PciIdDto {
        private String vendorId;
        private String deviceId;
        private String subsystemId;
        private String subsystemVendorId;

        public String getVendorId() {
            return vendorId;
        }

        public void setVendorId(String vendorId) {
            this.vendorId = vendorId;
        }

        public String getDeviceId() {
            return deviceId;
        }

        public void setDeviceId(String deviceId) {
            this.deviceId = deviceId;
        }

        public String getSubsystemId() {
            return subsystemId;
        }

        public void setSubsystemId(String subsystemId) {
            this.subsystemId = subsystemId;
        }

        public String getSubsystemVendorId() {
            return subsystemVendorId;
        }

        public void setSubsystemVendorId(String subsystemVendorId) {
            this.subsystemVendorId = subsystemVendorId;
        }
    }

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

        public Protocol getTransportProtocol() {
            return transportProtocol;
        }

        public void setTransportProtocol(Protocol transportProtocol) {
            this.transportProtocol = transportProtocol;
        }

        public IpV4AddressDto getIpv4Address() {
            return ipv4Address;
        }

        public void setIpv4Address(IpV4AddressDto ipv4Address) {
            this.ipv4Address = ipv4Address;
        }

        public IpV6AddressDto getIpv6Address() {
            return ipv6Address;
        }

        public void setIpv6Address(IpV6AddressDto ipv6Address) {
            this.ipv6Address = ipv6Address;
        }

        public Integer getPort() {
            return port;
        }

        public void setPort(Integer port) {
            this.port = port;
        }
    }

    public final class Actions extends RedfishActionsDto {

    }

    @JsonPropertyOrder({"@odata.type", "ports", "oem"})
    public final class Links extends RedfishLinksDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#Endpoint.v1_1_0.Links";
        private final Collection<Context> ports = new HashSet<>();
        private final Oem oem = new Oem();

        public Collection<Context> getPorts() {
            return ports;
        }

        public Oem getOem() {
            return oem;
        }

        @JsonInclude(NON_NULL)
        @OemType(OEM_IN_LINKS)
        public final class Oem extends RedfishOemDto {
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

            @JsonInclude(NON_NULL)
            @JsonPropertyOrder({"zones", "interfaces"})
            public final class RackScaleOem {
                private Set<Context> zones = new HashSet<>();
                private Set<Context> interfaces = new HashSet<>();

                public Set<Context> getZones() {
                    return zones;
                }

                public void setZones(Set<Context> zones) {
                    this.zones = zones;
                }

                public Set<Context> getInterfaces() {
                    return interfaces;
                }
            }
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "authentication"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Endpoint";
            private Authentication authentication = new Authentication();

            public Authentication getAuthentication() {
                return authentication;
            }

            public void setAuthentication(Authentication authentication) {
                this.authentication = authentication;
            }

            @JsonPropertyOrder({"username", "password"})
            public class Authentication {
                private String username;
                private String password;

                public String getUsername() {
                    return username;
                }

                public void setUsername(String username) {
                    this.username = username;
                }

                public String getPassword() {
                    return password;
                }

                public void setPassword(String password) {
                    this.password = password;
                }
            }
        }
    }

}
