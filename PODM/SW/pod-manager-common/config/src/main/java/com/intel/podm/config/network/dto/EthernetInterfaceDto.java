/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.config.network.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.Health;
import com.intel.podm.common.types.State;
import com.intel.podm.common.types.net.MacAddress;

import java.util.ArrayList;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public class EthernetInterfaceDto {
    @JsonProperty("Name")
    private String name;
    @JsonProperty("Description")
    private String description;
    @JsonProperty ("FQDN")
    private String fqdn;
    @JsonProperty ("HostName")
    private String hostName;
    @JsonProperty ("State")
    private State state;
    @JsonProperty ("Health")
    private Health health;
    @JsonProperty ("FactoryMacAddress")
    private MacAddress factoryMacAddress;
    @JsonProperty ("MacAddress")
    private MacAddress macAddress;
    @JsonProperty ("SpeedMbps")
    private Integer speedMbps;
    @JsonProperty ("Autosense")
    private Boolean autosense;
    @JsonProperty ("FullDuplex")
    private Boolean fullDuplex;
    @JsonProperty ("FrameSize")
    private Integer frameSize;
    @JsonProperty ("MaxIPv6StaticAddresses")
    private Integer maxIpV6StaticAddresses;
    @JsonProperty("VLANEnable")
    private Boolean vlanEnable;
    @JsonProperty("VLANId")
    private Integer vlanId;
    @JsonProperty ("IPv6DefaultGateway")
    private String ipV6DefaultGateway;
    @JsonProperty ("InterfaceEnabled")
    private Boolean interfaceEnabled;
    @JsonProperty ("IPv4Address")
    private NetworkIpV4AddressDto ipV4Address;

    private List<NetworkIpV4AddressDto> ipV4Addresses = new ArrayList<>();

    private List<VlanEthernetInterfaceDto> vlans = new ArrayList<>();

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public String getFqdn() {
        return fqdn;
    }

    public String getHostName() {
        return hostName;
    }

    public State getState() {
        return state;
    }

    public Health getHealth() {
        return health;
    }

    public MacAddress getFactoryMacAddress() {
        return factoryMacAddress;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public Integer getSpeedMbps() {
        return speedMbps;
    }

    public Boolean getAutosense() {
        return autosense;
    }

    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public Integer getFrameSize() {
        return frameSize;
    }

    public Integer getMaxIpV6StaticAddresses() {
        return maxIpV6StaticAddresses;
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public String getIpV6DefaultGateway() {
        return ipV6DefaultGateway;
    }

    public List<NetworkIpV4AddressDto> getIpV4Addresses() {
        return ipV4Addresses;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public void setFqdn(String fqdn) {
        this.fqdn = fqdn;
    }

    public void setHostName(String hostName) {
        this.hostName = hostName;
    }

    public void setState(State state) {
        this.state = state;
    }

    public void setHealth(Health health) {
        this.health = health;
    }

    public void setFactoryMacAddress(MacAddress factoryMacAddress) {
        this.factoryMacAddress = factoryMacAddress;
    }

    public void setMacAddress(MacAddress macAddress) {
        this.macAddress = macAddress;
    }

    public void setSpeedMbps(Integer speedMbps) {
        this.speedMbps = speedMbps;
    }

    public void setAutosense(Boolean autosense) {
        this.autosense = autosense;
    }

    public void setFullDuplex(Boolean fullDuplex) {
        this.fullDuplex = fullDuplex;
    }

    public void setFrameSize(Integer frameSize) {
        this.frameSize = frameSize;
    }

    public void setMaxIpV6StaticAddresses(Integer maxIpV6StaticAddresses) {
        this.maxIpV6StaticAddresses = maxIpV6StaticAddresses;
    }

    public void setVlanEnable(Boolean vlanEnable) {
        this.vlanEnable = vlanEnable;
    }

    public void setVlanId(Integer vlanId) {
        this.vlanId = vlanId;
    }

    public void setIpV6DefaultGateway(String ipV6DefaultGateway) {
        this.ipV6DefaultGateway = ipV6DefaultGateway;
    }

    public void setIpV4Addresses(List<NetworkIpV4AddressDto> ipV4Addresses) {
        this.ipV4Addresses = ipV4Addresses;
    }

    public NetworkIpV4AddressDto getIpV4Address() {
        return ipV4Address;
    }

    public Boolean getInterfaceEnabled() {
        return interfaceEnabled;
    }

    public void setInterfaceEnabled(Boolean interfaceEnabled) {
        this.interfaceEnabled = interfaceEnabled;
    }

    public List<VlanEthernetInterfaceDto> getVlans() {
        return vlans;
    }

    public void setVlans(List<VlanEthernetInterfaceDto> vlans) {
        this.vlans = vlans;
    }
}
