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
import com.intel.podm.client.api.resources.redfish.IscsiBootObject;
import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;

@SuppressWarnings({"checkstyle:MethodCount"})
public class IscsiBootObjectImpl implements IscsiBootObject {

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

    @Override
    public IpAddressType getIpAddressType() {
        return ipAddressType;
    }

    @Override
    public String getInitiatorIpAddress() {
        return initiatorIpAddress;
    }

    @Override
    public String getInitiatorName() {
        return initiatorName;
    }

    @Override
    public String getInitiatorDefaultGateway() {
        return initiatorDefaultGateway;
    }

    @Override
    public String getInitiatorNetmask() {
        return initiatorNetmask;
    }

    @Override
    public Boolean getTargetInfoViaDhcp() {
        return targetInfoViaDhcp;
    }

    @Override
    public String getPrimaryTargetName() {
        return primaryTargetName;
    }

    @Override
    public String getPrimaryTargetIpAddress() {
        return primaryTargetIpAddress;
    }

    @Override
    public Integer getPrimaryTargetTcpPort() {
        return primaryTargetTcpPort;
    }

    @Override
    public Integer getPrimaryLun() {
        return primaryLun;
    }

    @Override
    public Boolean getPrimaryVlanEnable() {
        return primaryVlanEnable;
    }

    @Override
    public Integer getPrimaryVlanId() {
        return primaryVlanId;
    }

    @Override
    public String getPrimaryDns() {
        return primaryDns;
    }

    @Override
    public String getSecondaryTargetName() {
        return secondaryTargetName;
    }

    @Override
    public String getSecondaryTargetIpAddress() {
        return secondaryTargetIpAddress;
    }

    @Override
    public Integer getSecondaryTargetTcpPort() {
        return secondaryTargetTcpPort;
    }

    @Override
    public Integer getSecondaryLun() {
        return secondaryLun;
    }

    @Override
    public Boolean getSecondaryVlanEnable() {
        return secondaryVlanEnable;
    }

    @Override
    public Integer getSecondaryVlanId() {
        return secondaryVlanId;
    }

    @Override
    public String getSecondaryDns() {
        return secondaryDns;
    }

    @Override
    public Boolean getIpMaskDnsViaDhcp() {
        return ipMaskDnsViaDhcp;
    }

    @Override
    public Boolean getRouterAdvertisementEnabled() {
        return routerAdvertisementEnabled;
    }

    @Override
    public AuthenticationMethod getAuthenticationMethod() {
        return authenticationMethod;
    }

    @Override
    public String getChapUsername() {
        return chapUsername;
    }

    @Override
    public String getChapSecret() {
        return chapSecret;
    }

    @Override
    public String getMutualChapUsername() {
        return mutualChapUsername;
    }

    @Override
    public String getMutualChapSecret() {
        return mutualChapSecret;
    }
}
