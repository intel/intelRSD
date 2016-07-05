/*
 * Copyright (c) 2016 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.EthernetSwitchPortResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.AdministrativeState;
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

@OdataTypes("#EthernetSwitchPort.1.0.0.EthernetSwitchPort")
public class EthernetSwitchPortResourceImpl extends ExternalServiceResourceImpl implements EthernetSwitchPortResource {
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
    private List<IpV4AddressObjectImpl> ipv4Addresses = new ArrayList<>();
    @JsonProperty("IPv6Addresses")
    private List<IpV6AddressObjectImpl> ipv6Addresses = new ArrayList<>();
    @JsonProperty("PortClass")
    private PortClass portClass;
    @JsonProperty("PortMode")
    private PortMode portMode;
    @JsonProperty("PortType")
    private PortType portType;
    @JsonProperty("VLANs")
    private ODataId vlans;
    @JsonProperty("Links")
    private Links links = new Links();
    @JsonProperty("Oem")
    private Object oem = new Object();

    @Override
    public String getPortId() {
        return portId;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public LinkType getLinkType() {
        return linkType;
    }

    @Override
    public OperationalState getOperationalState() {
        return operationalState;
    }

    @Override
    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    @Override
    public Integer getLinkSpeedMbps() {
        return linkSpeedMbps;
    }

    @Override
    public NeighborInfo getNeighborInfo() {
        return neighborInfo;
    }

    @Override
    public MacAddress getNeighborMac() {
        return neighborMac;
    }

    @Override
    public Integer getFrameSize() {
        return frameSize;
    }

    @Override
    public Boolean getAutosense() {
        return autosense;
    }

    @Override
    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    @Override
    public MacAddress getMacAddress() {
        return macAddress;
    }

    @Override
    public List<IpV4AddressObjectImpl> getIpV4Addresses() {
        return ipv4Addresses;
    }

    @Override
    public List<IpV6AddressObjectImpl> getIpV6Addresses() {
        return ipv6Addresses;
    }

    @Override
    public PortClass getPortClass() {
        return portClass;
    }

    @Override
    public PortMode getPortMode() {
        return portMode;
    }

    @Override
    public PortType getPortType() {
        return portType;
    }

    @Override
    @LinkName("primaryVlan")
    public ResourceSupplier getPrimaryVlan() {
        if (links.primaryVlan == null) {
            return null;
        }
        return toSupplier(links.primaryVlan);
    }

    @Override
    @LinkName("portMembers")
    public Iterable<ResourceSupplier> getPortMembers() throws ExternalServiceApiReaderException {
        return toSuppliers(links.portMembers);
    }

    @Override
    @LinkName("vlans")
    public Iterable<ResourceSupplier> getVlans() throws ExternalServiceApiReaderException {
        return processMembersListResource(vlans);
    }

    public Object getOem() {
        return oem;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static final class Links {
        @JsonProperty("PrimaryVLAN")
        private ODataId primaryVlan;
        @JsonProperty("Switch")
        private ODataId containedBySwitch;
        @JsonProperty("MemberOfPort")
        private ODataId memberOfPort;
        @JsonProperty("PortMembers")
        private List<ODataId> portMembers;
        @JsonProperty("Oem")
        private Object oem = new Object();
    }
}
