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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DurableNameFormat;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.EntityType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.DurableIdentifier;

import java.util.Collection;
import java.util.HashSet;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status",
    "protocol", "pciId", "identifiers", "connectedEntities", "redundancies",
    "hostReservationMemoryBytes", "links", "oem", "actions"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class EndpointDto extends RedfishDto {
    private Status status;
    @JsonProperty("EndpointProtocol")
    private Protocol protocol;
    private PciIdDto pciId;
    private Collection<IdentifierDto> identifiers = new HashSet<>();
    private final Collection<ConnectedEntityDto> connectedEntities = new HashSet<>();
    @JsonProperty("Redundancy")
    private final Collection<RedundancyDto> redundancies = new HashSet<>();
    private Integer hostReservationMemoryBytes;
    private final Actions actions = new Actions();
    private final Links links = new Links();

    public EndpointDto() {
        super("#Endpoint.v1_0_0.Endpoint");
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

    public Collection<ConnectedEntityDto> getConnectedEntities() {
        return connectedEntities;
    }

    public Collection<RedundancyDto> getRedundancies() {
        return redundancies;
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

    public final class Actions extends RedfishActionsDto {

    }

    @JsonPropertyOrder({"ports", "oem"})
    public final class Links extends RedfishLinksDto {
        @JsonProperty("Ports")
        private final Collection<Context> ports = new HashSet<>();

        public Collection<Context> getPorts() {
            return ports;
        }
    }

    @JsonPropertyOrder({"durableName", "durableNameFormat"})
    public static final class IdentifierDto implements DurableIdentifier {
        @JsonProperty("DurableName")
        private String durableName;
        @JsonProperty("DurableNameFormat")
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

    @JsonPropertyOrder({
        "entityType", "entityRole", "pciFunctionNumber", "pciClassCode", "pciId", "entityLink", "identifiers", "oem"
    })
    @SuppressWarnings({"checkstyle:MethodCount"})
    public static final class ConnectedEntityDto {
        @JsonProperty("EntityType")
        private EntityType entityType;
        @JsonProperty("EntityRole")
        private EntityRole entityRole;
        @JsonProperty("PciFunctionNumber")
        private Integer pciFunctionNumber;
        @JsonProperty("PciClassCode")
        private String pciClassCode;
        @JsonProperty("EntityPciId")
        private PciIdDto pciId;
        @JsonProperty("Identifiers")
        private Collection<IdentifierDto> identifiers = new HashSet<>();
        @JsonProperty("EntityLink")
        private Context entityLink;

        public EntityType getEntityType() {
            return entityType;
        }

        public void setEntityType(EntityType entityType) {
            this.entityType = entityType;
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

    @JsonPropertyOrder({"VendorId", "DeviceId", "SubsystemId", "SubsystemVendorId"})
    public static final class PciIdDto {
        @JsonProperty("VendorId")
        private String vendorId;

        @JsonProperty("DeviceId")
        private String deviceId;

        @JsonProperty("SubsystemId")
        private String subsystemId;

        @JsonProperty("SubsystemVendorId")
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
}
