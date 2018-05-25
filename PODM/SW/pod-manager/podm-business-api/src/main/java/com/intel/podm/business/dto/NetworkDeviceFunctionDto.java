/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "name", "description", "id", "status", "deviceEnabled", "ethernet", "iscsiBoot", "links", "oem"
})
public final class NetworkDeviceFunctionDto extends RedfishDto {
    @IgnoreAutomaticOem
    private final Object links = new Object();
    private Status status;
    private Boolean deviceEnabled;
    private EthernetJson ethernet = new EthernetJson();
    @JsonProperty("iSCSIBoot")
    private IscsiBootJson iscsiBoot = new IscsiBootJson();

    public NetworkDeviceFunctionDto() {
        super("#NetworkDeviceFunction.v1_0_0.NetworkDeviceFunction");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Boolean getDeviceEnabled() {
        return deviceEnabled;
    }

    public void setDeviceEnabled(Boolean deviceEnabled) {
        this.deviceEnabled = deviceEnabled;
    }

    public EthernetJson getEthernet() {
        return ethernet;
    }

    public void setEthernet(EthernetJson ethernet) {
        this.ethernet = ethernet;
    }

    public IscsiBootJson getIscsiBoot() {
        return iscsiBoot;
    }

    public void setIscsiBoot(IscsiBootJson iscsiBoot) {
        this.iscsiBoot = iscsiBoot;
    }

    public Object getLinks() {
        return links;
    }

    @JsonPropertyOrder({"macAddress"})
    public static class EthernetJson {
        @JsonProperty("MACAddress")
        private MacAddress macAddress;

        public MacAddress getMacAddress() {
            return macAddress;
        }

        public void setMacAddress(MacAddress macAddress) {
            this.macAddress = macAddress;
        }
    }

    @JsonPropertyOrder({
        "ipAddressType", "initiatorIpAddress", "initiatorName", "initiatorDefaultGateway", "initiatorNetmask", "targetInfoViaDhcp", "primaryTargetName",
        "primaryTargetIpAddress", "primaryTargetTcpPort", "primaryLun", "primaryVlanEnable", "primaryVlanId", "primaryDns", "secondaryTargetName",
        "secondaryTargetIpAddress", "secondaryTargetTcpPort", "secondaryLun", "secondaryVlanEnable", "secondaryVlanId", "secondaryDns", "ipMaskDnsViaDhcp",
        "routerAdvertisementEnabled", "authenticationMethod", "chapUsername", "chapSecret", "mutualChapUsername", "mutualChapSecret"
    })
    @SuppressWarnings({"checkstyle:MethodCount"})
    public static class IscsiBootJson {
        @JsonProperty("IPAddressType")
        private IpAddressType ipAddressType;
        @JsonProperty("InitiatorIPAddress")
        private String initiatorIpAddress;
        private String initiatorName;
        private String initiatorDefaultGateway;
        private String initiatorNetmask;
        @JsonProperty("TargetInfoViaDHCP")
        private Boolean targetInfoViaDhcp;
        private String primaryTargetName;
        @JsonProperty("PrimaryTargetIPAddress")
        private String primaryTargetIpAddress;
        @JsonProperty("PrimaryTargetTCPPort")
        private Integer primaryTargetTcpPort;
        @JsonProperty("PrimaryLUN")
        private Integer primaryLun;
        @JsonProperty("PrimaryVLANEnable")
        private Boolean primaryVlanEnable;
        @JsonProperty("PrimaryVLANId")
        private Integer primaryVlanId;
        @JsonProperty("PrimaryDNS")
        private String primaryDns;
        private String secondaryTargetName;
        @JsonProperty("SecondaryTargetIPAddress")
        private String secondaryTargetIpAddress;
        @JsonProperty("SecondaryTargetTCPPort")
        private Integer secondaryTargetTcpPort;
        @JsonProperty("SecondaryLUN")
        private Integer secondaryLun;
        @JsonProperty("SecondaryVLANEnable")
        private Boolean secondaryVlanEnable;
        @JsonProperty("SecondaryVLANId")
        private Integer secondaryVlanId;
        @JsonProperty("SecondaryDNS")
        private String secondaryDns;
        @JsonProperty("IPMaskDNSViaDHCP")
        private Boolean ipMaskDnsViaDhcp;
        private Boolean routerAdvertisementEnabled;
        private AuthenticationMethod authenticationMethod;
        @JsonProperty("CHAPUsername")
        private String chapUsername;
        @JsonProperty("CHAPSecret")
        private String chapSecret;
        @JsonProperty("MutualCHAPUsername")
        private String mutualChapUsername;
        @JsonProperty("MutualCHAPSecret")
        private String mutualChapSecret;

        public IpAddressType getIpAddressType() {
            return ipAddressType;
        }

        public void setIpAddressType(IpAddressType ipAddressType) {
            this.ipAddressType = ipAddressType;
        }

        public String getInitiatorIpAddress() {
            return initiatorIpAddress;
        }

        public void setInitiatorIpAddress(String initiatorIpAddress) {
            this.initiatorIpAddress = initiatorIpAddress;
        }

        public String getInitiatorName() {
            return initiatorName;
        }

        public void setInitiatorName(String initiatorName) {
            this.initiatorName = initiatorName;
        }

        public String getInitiatorDefaultGateway() {
            return initiatorDefaultGateway;
        }

        public void setInitiatorDefaultGateway(String initiatorDefaultGateway) {
            this.initiatorDefaultGateway = initiatorDefaultGateway;
        }

        public String getInitiatorNetmask() {
            return initiatorNetmask;
        }

        public void setInitiatorNetmask(String initiatorNetmask) {
            this.initiatorNetmask = initiatorNetmask;
        }

        public Boolean getTargetInfoViaDhcp() {
            return targetInfoViaDhcp;
        }

        public void setTargetInfoViaDhcp(Boolean targetInfoViaDhcp) {
            this.targetInfoViaDhcp = targetInfoViaDhcp;
        }

        public String getPrimaryTargetName() {
            return primaryTargetName;
        }

        public void setPrimaryTargetName(String primaryTargetName) {
            this.primaryTargetName = primaryTargetName;
        }

        public String getPrimaryTargetIpAddress() {
            return primaryTargetIpAddress;
        }

        public void setPrimaryTargetIpAddress(String primaryTargetIpAddress) {
            this.primaryTargetIpAddress = primaryTargetIpAddress;
        }

        public Integer getPrimaryTargetTcpPort() {
            return primaryTargetTcpPort;
        }

        public void setPrimaryTargetTcpPort(Integer primaryTargetTcpPort) {
            this.primaryTargetTcpPort = primaryTargetTcpPort;
        }

        public Integer getPrimaryLun() {
            return primaryLun;
        }

        public void setPrimaryLun(Integer primaryLun) {
            this.primaryLun = primaryLun;
        }

        public Boolean getPrimaryVlanEnable() {
            return primaryVlanEnable;
        }

        public void setPrimaryVlanEnable(Boolean primaryVlanEnable) {
            this.primaryVlanEnable = primaryVlanEnable;
        }

        public Integer getPrimaryVlanId() {
            return primaryVlanId;
        }

        public void setPrimaryVlanId(Integer primaryVlanId) {
            this.primaryVlanId = primaryVlanId;
        }

        public String getPrimaryDns() {
            return primaryDns;
        }

        public void setPrimaryDns(String primaryDns) {
            this.primaryDns = primaryDns;
        }

        public String getSecondaryTargetName() {
            return secondaryTargetName;
        }

        public void setSecondaryTargetName(String secondaryTargetName) {
            this.secondaryTargetName = secondaryTargetName;
        }

        public String getSecondaryTargetIpAddress() {
            return secondaryTargetIpAddress;
        }

        public void setSecondaryTargetIpAddress(String secondaryTargetIpAddress) {
            this.secondaryTargetIpAddress = secondaryTargetIpAddress;
        }

        public Integer getSecondaryTargetTcpPort() {
            return secondaryTargetTcpPort;
        }

        public void setSecondaryTargetTcpPort(Integer secondaryTargetTcpPort) {
            this.secondaryTargetTcpPort = secondaryTargetTcpPort;
        }

        public Integer getSecondaryLun() {
            return secondaryLun;
        }

        public void setSecondaryLun(Integer secondaryLun) {
            this.secondaryLun = secondaryLun;
        }

        public Boolean getSecondaryVlanEnable() {
            return secondaryVlanEnable;
        }

        public void setSecondaryVlanEnable(Boolean secondaryVlanEnable) {
            this.secondaryVlanEnable = secondaryVlanEnable;
        }

        public Integer getSecondaryVlanId() {
            return secondaryVlanId;
        }

        public void setSecondaryVlanId(Integer secondaryVlanId) {
            this.secondaryVlanId = secondaryVlanId;
        }

        public String getSecondaryDns() {
            return secondaryDns;
        }

        public void setSecondaryDns(String secondaryDns) {
            this.secondaryDns = secondaryDns;
        }

        public Boolean getIpMaskDnsViaDhcp() {
            return ipMaskDnsViaDhcp;
        }

        public void setIpMaskDnsViaDhcp(Boolean ipMaskDnsViaDhcp) {
            this.ipMaskDnsViaDhcp = ipMaskDnsViaDhcp;
        }

        public Boolean getRouterAdvertisementEnabled() {
            return routerAdvertisementEnabled;
        }

        public void setRouterAdvertisementEnabled(Boolean routerAdvertisementEnabled) {
            this.routerAdvertisementEnabled = routerAdvertisementEnabled;
        }

        public AuthenticationMethod getAuthenticationMethod() {
            return authenticationMethod;
        }

        public void setAuthenticationMethod(AuthenticationMethod authenticationMethod) {
            this.authenticationMethod = authenticationMethod;
        }

        public String getChapUsername() {
            return chapUsername;
        }

        public void setChapUsername(String chapUsername) {
            this.chapUsername = chapUsername;
        }

        public String getChapSecret() {
            return chapSecret;
        }

        public void setChapSecret(String chapSecret) {
            this.chapSecret = chapSecret;
        }

        public String getMutualChapUsername() {
            return mutualChapUsername;
        }

        public void setMutualChapUsername(String mutualChapUsername) {
            this.mutualChapUsername = mutualChapUsername;
        }

        public String getMutualChapSecret() {
            return mutualChapSecret;
        }

        public void setMutualChapSecret(String mutualChapSecret) {
            this.mutualChapSecret = mutualChapSecret;
        }
    }
}
