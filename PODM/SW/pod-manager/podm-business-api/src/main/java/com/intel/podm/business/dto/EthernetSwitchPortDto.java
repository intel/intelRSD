/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.DcbxState;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.RedfishErrorResponse.Error.ExtendedInfo;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchPort;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "portId", "status", "linkType", "operationalState", "administrativeState",
    "linkSpeedMbps", "neighborInfo", "neighborMac", "frameSize", "autosense", "fullDuplex", "macAddress", "ipV4Addresses", "ipV6Addresses", "portClass",
    "portMode", "portType", "vlans", "staticMacs", "metrics", "neighborInterface", "neighborInterfaceExtendedInfo", "dcbxState", "lldpEnabled",
    "priorityFlowControl", "links", "oem"
})
@JsonIgnoreProperties({"Oem"})
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public final class EthernetSwitchPortDto extends RedfishDto implements RedfishEthernetSwitchPort {
    private final Links links = new Links();
    private String portId;
    private Status status;
    private LinkType linkType;
    private OperationalState operationalState;
    private AdministrativeState administrativeState;
    private Integer linkSpeedMbps;
    private NeighborInfo neighborInfo;
    @JsonProperty("NeighborMAC")
    private MacAddress neighborMac;
    private Integer frameSize;
    private Boolean autosense;
    private Boolean fullDuplex;
    @JsonProperty("MACAddress")
    private MacAddress macAddress;
    @JsonProperty("IPv4Addresses")
    private List<IpAddresses.IpV4AddressDto> ipV4Addresses = new ArrayList<>();
    @JsonProperty("IPv6Addresses")
    private List<IpAddresses.IpV6AddressDto> ipV6Addresses = new ArrayList<>();
    private PortClass portClass;
    private PortMode portMode;
    private PortType portType;
    @JsonProperty("VLANs")
    private SingletonContext vlans;
    @JsonProperty("StaticMACs")
    private SingletonContext staticMacs;
    @JsonInclude(NON_NULL)
    private SingletonContext metrics;
    private Context neighborInterface;
    @JsonInclude(NON_NULL)
    @JsonProperty("NeighborInterface@Message.ExtendedInfo")
    private List<ExtendedInfo> neighborInterfaceExtendedInfo;
    @JsonProperty("DCBXState")
    private DcbxState dcbxState;
    @JsonProperty("LLDPEnabled")
    private Boolean lldpEnabled;
    private PriorityFlowControlDto priorityFlowControl;

    public EthernetSwitchPortDto() {
        super("#EthernetSwitchPort.v1_1_0.EthernetSwitchPort");
    }

    @Override
    public String getPortId() {
        return portId;
    }

    public void setPortId(String portId) {
        this.portId = portId;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    @Override
    public LinkType getLinkType() {
        return linkType;
    }

    public void setLinkType(LinkType linkType) {
        this.linkType = linkType;
    }

    @Override
    public OperationalState getOperationalState() {
        return operationalState;
    }

    public void setOperationalState(OperationalState operationalState) {
        this.operationalState = operationalState;
    }

    @Override
    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    public void setAdministrativeState(AdministrativeState administrativeState) {
        this.administrativeState = administrativeState;
    }

    @Override
    public Integer getLinkSpeedMbps() {
        return linkSpeedMbps;
    }

    public void setLinkSpeedMbps(Integer linkSpeedMbps) {
        this.linkSpeedMbps = linkSpeedMbps;
    }

    @Override
    public NeighborInfo getNeighborInfo() {
        return neighborInfo;
    }

    public void setNeighborInfo(NeighborInfo neighborInfo) {
        this.neighborInfo = neighborInfo;
    }

    @Override
    public MacAddress getNeighborMac() {
        return neighborMac;
    }

    public void setNeighborMac(MacAddress neighborMac) {
        this.neighborMac = neighborMac;
    }

    @Override
    public Integer getFrameSize() {
        return frameSize;
    }

    public void setFrameSize(Integer frameSize) {
        this.frameSize = frameSize;
    }

    @Override
    public Boolean getAutosense() {
        return autosense;
    }

    public void setAutosense(Boolean autosense) {
        this.autosense = autosense;
    }

    @Override
    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public void setFullDuplex(Boolean fullDuplex) {
        this.fullDuplex = fullDuplex;
    }

    @Override
    public MacAddress getMacAddress() {
        return macAddress;
    }

    public void setMacAddress(MacAddress macAddress) {
        this.macAddress = macAddress;
    }

    public List<IpAddresses.IpV4AddressDto> getIpV4Addresses() {
        return ipV4Addresses;
    }

    public void setIpV4Addresses(List<IpAddresses.IpV4AddressDto> ipV4Addresses) {
        this.ipV4Addresses = ipV4Addresses;
    }

    public List<IpAddresses.IpV6AddressDto> getIpV6Addresses() {
        return ipV6Addresses;
    }

    public void setIpV6Addresses(List<IpAddresses.IpV6AddressDto> ipV6Addresses) {
        this.ipV6Addresses = ipV6Addresses;
    }

    @Override
    public PortClass getPortClass() {
        return portClass;
    }

    public void setPortClass(PortClass portClass) {
        this.portClass = portClass;
    }

    @Override
    public PortMode getPortMode() {
        return portMode;
    }

    public void setPortMode(PortMode portMode) {
        this.portMode = portMode;
    }

    @Override
    public PortType getPortType() {
        return portType;
    }

    public void setPortType(PortType portType) {
        this.portType = portType;
    }

    public SingletonContext getVlans() {
        return vlans;
    }

    public void setVlans(SingletonContext vlans) {
        this.vlans = vlans;
    }

    public SingletonContext getStaticMacs() {
        return staticMacs;
    }

    public void setStaticMacs(SingletonContext staticMacs) {
        this.staticMacs = staticMacs;
    }

    public SingletonContext getMetrics() {
        return metrics;
    }

    public void setMetrics(SingletonContext metrics) {
        this.metrics = metrics;
    }

    public Context getNeighborInterface() {
        return neighborInterface;
    }

    public void setNeighborInterface(Context neighborInterface) {
        this.neighborInterface = neighborInterface;
    }

    public List<ExtendedInfo> getNeighborInterfaceExtendedInfo() {
        return neighborInterfaceExtendedInfo;
    }

    public void setNeighborInterfaceExtendedInfo(List<ExtendedInfo> neighborInterfaceExtendedInfo) {
        this.neighborInterfaceExtendedInfo = neighborInterfaceExtendedInfo;
    }

    public DcbxState getDcbxState() {
        return dcbxState;
    }

    public void setDcbxState(DcbxState dcbxState) {
        this.dcbxState = dcbxState;
    }

    public Boolean getLldpEnabled() {
        return lldpEnabled;
    }

    public void setLldpEnabled(Boolean lldpEnabled) {
        this.lldpEnabled = lldpEnabled;
    }

    public PriorityFlowControlDto getPriorityFlowControl() {
        return priorityFlowControl;
    }

    public void setPriorityFlowControl(PriorityFlowControlDto priorityFlowControl) {
        this.priorityFlowControl = priorityFlowControl;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    @JsonPropertyOrder({"primaryVlan", "switchContext", "memberOfPort", "portMembers", "activeAcls", "oem"})
    public class Links extends RedfishDto.RedfishLinksDto implements RedfishEthernetSwitchPort.Links {
        private final Set<Context> portMembers = new HashSet<>();
        @JsonProperty("ActiveACLs")
        private final Set<Context> activeAcls = new HashSet<>();
        @JsonProperty("PrimaryVLAN")
        private Context primaryVlan;
        @JsonProperty("Switch")
        private Context switchContext;
        private Context memberOfPort;

        @Override
        public Context getPrimaryVlan() {
            return primaryVlan;
        }

        public void setPrimaryVlan(Context primaryVlan) {
            this.primaryVlan = primaryVlan;
        }

        @Override
        public Context getSwitchContext() {
            return switchContext;
        }

        public void setSwitchContext(Context switchContext) {
            this.switchContext = switchContext;
        }

        @Override
        public Context getMemberOfPort() {
            return memberOfPort;
        }

        public void setMemberOfPort(Context memberOfPort) {
            this.memberOfPort = memberOfPort;
        }

        @Override
        public Set<Context> getPortMembers() {
            return portMembers;
        }

        public Set<Context> getActiveAcls() {
            return activeAcls;
        }
    }

    @JsonPropertyOrder({"enabled", "enabledPriorities"})
    public static final class PriorityFlowControlDto implements PriorityFlowControl {
        private Boolean enabled;
        private List<Integer> enabledPriorities = new ArrayList<>();

        @Override
        public Boolean getEnabled() {
            return enabled;
        }

        public void setEnabled(Boolean enabled) {
            this.enabled = enabled;
        }

        @Override
        public List<Integer> getEnabledPriorities() {
            return enabledPriorities;
        }

        public void setEnabledPriorities(List<Integer> enabledPriorities) {
            this.enabledPriorities = enabledPriorities;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            PriorityFlowControlDto that = (PriorityFlowControlDto) o;
            return new EqualsBuilder()
                .append(enabled, that.enabled)
                .append(enabledPriorities, that.enabledPriorities)
                .isEquals();
        }

        public int hashCode() {
            return new HashCodeBuilder()
                .append(enabled)
                .append(enabledPriorities)
                .toHashCode();
        }
    }
}
