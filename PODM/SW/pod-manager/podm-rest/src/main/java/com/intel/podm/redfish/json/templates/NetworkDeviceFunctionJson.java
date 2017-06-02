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

package com.intel.podm.redfish.json.templates;

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
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class NetworkDeviceFunctionJson extends BaseResourceJson {
    public Status status;
    public Boolean deviceEnabled;
    public EthernetJson ethernet = new EthernetJson();
    @JsonProperty("iSCSIBoot")
    public IscsiBootJson iscsiBoot = new IscsiBootJson();
    @IgnoreAutomaticOem
    public Links links = new Links();

    public NetworkDeviceFunctionJson() {
        super("#NetworkDeviceFunction.v1_0_0.NetworkDeviceFunction");
    }

    public final class Links {
    }

    @JsonPropertyOrder({
        "macAddress"
    })
    public static class EthernetJson {
        @JsonProperty("MACAddress")
        public MacAddress macAddress;
    }

    @JsonPropertyOrder({
        "ipAddressType", "initiatorIpAddress", "initiatorName", "initiatorDefaultGateway", "initiatorNetmask", "targetInfoViaDhcp",
        "primaryTargetName", "primaryTargetIpAddress", "primaryTargetTcpPort", "primaryLun", "primaryVlanEnable", "primaryVlanId",
        "primaryDns", "secondaryTargetName", "secondaryTargetIpAddress", "secondaryTargetTcpPort", "secondaryLun", "secondaryVlanEnable",
        "secondaryVlanId", "secondaryDns", "ipMaskDnsViaDhcp", "routerAdvertisementEnabled", "authenticationMethod", "chapUsername",
        "chapSecret", "mutualChapUsername", "mutualChapSecret"
    })
    public static class IscsiBootJson {
        @JsonProperty("IPAddressType")
        public IpAddressType ipAddressType;
        @JsonProperty("InitiatorIPAddress")
        public String initiatorIpAddress;
        public String initiatorName;
        public String initiatorDefaultGateway;
        public String initiatorNetmask;
        @JsonProperty("TargetInfoViaDHCP")
        public Boolean targetInfoViaDhcp;
        public String primaryTargetName;
        @JsonProperty("PrimaryTargetIPAddress")
        public String primaryTargetIpAddress;
        @JsonProperty("PrimaryTargetTCPPort")
        public Integer primaryTargetTcpPort;
        @JsonProperty("PrimaryLUN")
        public Integer primaryLun;
        @JsonProperty("PrimaryVLANEnable")
        public Boolean primaryVlanEnable;
        @JsonProperty("PrimaryVLANId")
        public Integer primaryVlanId;
        @JsonProperty("PrimaryDNS")
        public String primaryDns;
        public String secondaryTargetName;
        @JsonProperty("SecondaryTargetIPAddress")
        public String secondaryTargetIpAddress;
        @JsonProperty("SecondaryTargetTCPPort")
        public Integer secondaryTargetTcpPort;
        @JsonProperty("SecondaryLUN")
        public Integer secondaryLun;
        @JsonProperty("SecondaryVLANEnable")
        public Boolean secondaryVlanEnable;
        @JsonProperty("SecondaryVLANId")
        public Integer secondaryVlanId;
        @JsonProperty("SecondaryDNS")
        public String secondaryDns;
        @JsonProperty("IPMaskDNSViaDHCP")
        public Boolean ipMaskDnsViaDhcp;
        public Boolean routerAdvertisementEnabled;
        public AuthenticationMethod authenticationMethod;
        @JsonProperty("CHAPUsername")
        public String chapUsername;
        @JsonProperty("CHAPSecret")
        public String chapSecret;
        @JsonProperty("MutualCHAPUsername")
        public String mutualChapUsername;
        @JsonProperty("MutualCHAPSecret")
        public String mutualChapSecret;
    }
}
