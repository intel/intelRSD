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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
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

import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#EthernetSwitchPort" + VERSION_PATTERN + "EthernetSwitchPort"
})
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class EthernetSwitchPortResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("PortId")
    private String portId;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("LinkType")
    private LinkType linkType;
    @JsonProperty("OperationalState")
    private OperationalState operationalState;
    @JsonProperty("AdministrativeState")
    private AdministrativeState administrativeState;
    @JsonProperty("LinkSpeedMbps")
    private Integer linkSpeedMbps;
    @JsonProperty("NeighborInfo")
    private NeighborInfo neighborInfo;
    @JsonProperty("NeighborMAC")
    private MacAddress neighborMac;
    @JsonProperty("FrameSize")
    private Integer frameSize;
    @JsonProperty("Autosense")
    private Boolean autosense;
    @JsonProperty("FullDuplex")
    private Boolean fullDuplex;
    @JsonProperty("MACAddress")
    private MacAddress macAddress;
    @JsonProperty("IPv4Addresses")
    private List<IpV4AddressObject> ipv4Addresses = new ArrayList<>();
    @JsonProperty("IPv6Addresses")
    private List<IpV6AddressObject> ipv6Addresses = new ArrayList<>();
    @JsonProperty("PortClass")
    private PortClass portClass;
    @JsonProperty("PortMode")
    private PortMode portMode;
    @JsonProperty("PortType")
    private PortType portType;
    @JsonProperty("VLANs")
    private ODataId vlans;
    @JsonProperty("StaticMACs")
    private ODataId staticMacs;
    @JsonProperty("Metrics")
    private ODataId metrics;
    @JsonProperty("Links")
    private Links links = new Links();
    @JsonProperty("DCBXState")
    private DcbxState dcbxState;
    @JsonProperty("LLDPEnabled")
    private Boolean lldpEnabled;
    @JsonProperty("PriorityFlowControl")
    private PriorityFlowControlObject priorityFlowControl;

    public String getPortId() {
        return portId;
    }

    public Status getStatus() {
        return status;
    }

    public LinkType getLinkType() {
        return linkType;
    }

    public OperationalState getOperationalState() {
        return operationalState;
    }

    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    public Integer getLinkSpeedMbps() {
        return linkSpeedMbps;
    }

    public NeighborInfo getNeighborInfo() {
        return neighborInfo;
    }

    public MacAddress getNeighborMac() {
        return neighborMac;
    }

    public Integer getFrameSize() {
        return frameSize;
    }

    public Boolean getAutosense() {
        return autosense;
    }

    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public List<IpV4AddressObject> getIpV4Addresses() {
        return ipv4Addresses;
    }

    public List<IpV6AddressObject> getIpV6Addresses() {
        return ipv6Addresses;
    }

    public PortClass getPortClass() {
        return portClass;
    }

    public PortMode getPortMode() {
        return portMode;
    }

    public PortType getPortType() {
        return portType;
    }

    public DcbxState getDcbxState() {
        return dcbxState;
    }

    public Boolean getLldpEnabled() {
        return lldpEnabled;
    }

    public PriorityFlowControlObject getPriorityFlowControl() {
        return priorityFlowControl;
    }

    @LinkName("primaryVlan")
    public ResourceSupplier getPrimaryVlan() {
        if (links.primaryVlan == null) {
            return null;
        }
        return toSupplier(links.primaryVlan);
    }

    @LinkName("portMembers")
    public Iterable<ResourceSupplier> getPortMembers() throws WebClientRequestException {
        return toSuppliers(links.portMembers);
    }

    @LinkName("vlans")
    public Iterable<ResourceSupplier> getVlans() throws WebClientRequestException {
        return processMembersListResource(vlans);
    }

    @LinkName("staticMacs")
    public Iterable<ResourceSupplier> getStaticMacs() throws WebClientRequestException {
        return processMembersListResource(staticMacs);
    }

    @LinkName("switchPortMetrics")
    public ResourceSupplier getMetrics() throws WebClientRequestException {
        if (metrics == null) {
            return null;
        }
        return toSupplier(metrics);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("PrimaryVLAN")
        private ODataId primaryVlan;
        @JsonProperty("Switch")
        private ODataId containedBySwitch;
        @JsonProperty("MemberOfPort")
        private ODataId memberOfPort;
        @JsonProperty("PortMembers")
        private List<ODataId> portMembers;
    }
}
