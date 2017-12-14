/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;

@SuppressWarnings({"checkstyle:MethodCount"})
public class IscsiBootObject {
    @JsonProperty("IPAddressType")
    private IpAddressType ipAddressType;

    @JsonProperty("InitiatorIPAddress")
    private String initiatorIpAddress;

    @JsonProperty("InitiatorName")
    private String initiatorName;

    @JsonProperty("InitiatorDefaultGateway")
    private String initiatorDefaultGateway;

    @JsonProperty("InitiatorNetmask")
    private String initiatorNetmask;

    @JsonProperty("TargetInfoViaDHCP")
    private Boolean targetInfoViaDhcp;

    @JsonProperty("PrimaryTargetName")
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

    @JsonProperty("SecondaryTargetName")
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

    @JsonProperty("RouterAdvertisementEnabled")
    private Boolean routerAdvertisementEnabled;

    @JsonProperty("AuthenticationMethod")
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

    public String getInitiatorIpAddress() {
        return initiatorIpAddress;
    }

    public String getInitiatorName() {
        return initiatorName;
    }

    public String getInitiatorDefaultGateway() {
        return initiatorDefaultGateway;
    }

    public String getInitiatorNetmask() {
        return initiatorNetmask;
    }

    public Boolean getTargetInfoViaDhcp() {
        return targetInfoViaDhcp;
    }

    public String getPrimaryTargetName() {
        return primaryTargetName;
    }

    public String getPrimaryTargetIpAddress() {
        return primaryTargetIpAddress;
    }

    public Integer getPrimaryTargetTcpPort() {
        return primaryTargetTcpPort;
    }

    public Integer getPrimaryLun() {
        return primaryLun;
    }

    public Boolean getPrimaryVlanEnable() {
        return primaryVlanEnable;
    }

    public Integer getPrimaryVlanId() {
        return primaryVlanId;
    }

    public String getPrimaryDns() {
        return primaryDns;
    }

    public String getSecondaryTargetName() {
        return secondaryTargetName;
    }

    public String getSecondaryTargetIpAddress() {
        return secondaryTargetIpAddress;
    }

    public Integer getSecondaryTargetTcpPort() {
        return secondaryTargetTcpPort;
    }

    public Integer getSecondaryLun() {
        return secondaryLun;
    }

    public Boolean getSecondaryVlanEnable() {
        return secondaryVlanEnable;
    }

    public Integer getSecondaryVlanId() {
        return secondaryVlanId;
    }

    public String getSecondaryDns() {
        return secondaryDns;
    }

    public Boolean getIpMaskDnsViaDhcp() {
        return ipMaskDnsViaDhcp;
    }

    public Boolean getRouterAdvertisementEnabled() {
        return routerAdvertisementEnabled;
    }

    public AuthenticationMethod getAuthenticationMethod() {
        return authenticationMethod;
    }

    public String getChapUsername() {
        return chapUsername;
    }

    public String getChapSecret() {
        return chapSecret;
    }

    public String getMutualChapUsername() {
        return mutualChapUsername;
    }

    public String getMutualChapSecret() {
        return mutualChapSecret;
    }
}
