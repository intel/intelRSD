/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.types.AuthenticationMethod;
import com.intel.rsd.nodecomposer.types.IpAddressType;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.actions.NetworkDeviceFunctionUpdateDefinition;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public class NetworkDeviceFunctionRequest {
    @JsonProperty("DeviceEnabled")
    private Ref<Boolean> deviceEnabled;

    @JsonProperty("iSCSIBoot")
    private Ref<IscsiBoot> iscsiBoot;

    @JsonProperty("Ethernet")
    private Ref<Ethernet> ethernet;

    public NetworkDeviceFunctionRequest(NetworkDeviceFunctionUpdateDefinition networkDeviceUpdateDefinition) {
        networkDeviceUpdateDefinition.getDeviceEnabled().ifAssigned(deviceEnabled -> this.deviceEnabled = networkDeviceUpdateDefinition.getDeviceEnabled());
        networkDeviceUpdateDefinition.getIscsiBoot().ifAssigned(iscsiBootDefinition -> this.iscsiBoot = Ref.of(new IscsiBoot(iscsiBootDefinition)));
        networkDeviceUpdateDefinition.getEthernet().ifAssigned(ethernetDefinition -> this.ethernet = Ref.of(new Ethernet(ethernetDefinition)));
    }

    @JsonInclude(NON_NULL)
    public static class Ethernet {

        @JsonProperty("MACAddress")
        private Ref<String> macAddress;

        public Ethernet(NetworkDeviceFunctionUpdateDefinition.EthernetDefinition iscsiEthernetDefinition) {
            if (iscsiEthernetDefinition == null) {
                return;
            }

            macAddress = iscsiEthernetDefinition.getMacAddress();
        }
    }

    @JsonInclude(NON_NULL)
    public static class IscsiBoot {
        @JsonProperty("IPAddressType")
        private Ref<IpAddressType> ipAddressType;

        @JsonProperty("InitiatorIPAddress")
        private Ref<String> initiatorIpAddress;

        @JsonProperty("InitiatorName")
        private Ref<String> initiatorName;

        @JsonProperty("InitiatorDefaultGateway")
        private Ref<String> initiatorDefaultGateway;

        @JsonProperty("InitiatorNetmask")
        private Ref<String> initiatorNetmask;

        @JsonProperty("TargetInfoViaDHCP")
        private Ref<Boolean> targetInfoViaDhcp;

        @JsonProperty("PrimaryTargetName")
        private Ref<String> primaryTargetName;

        @JsonProperty("PrimaryTargetIPAddress")
        private Ref<String> primaryTargetIpAddress;

        @JsonProperty("PrimaryTargetTCPPort")
        private Ref<Integer> primaryTargetTcpPort;

        @JsonProperty("PrimaryLUN")
        private Ref<Integer> primaryLun;

        @JsonProperty("PrimaryVLANEnable")
        private Ref<Boolean> primaryVlanEnable;

        @JsonProperty("PrimaryVLANId")
        private Ref<Integer> primaryVlanId;

        @JsonProperty("PrimaryDNS")
        private Ref<String> primaryDns;

        @JsonProperty("SecondaryTargetName")
        private Ref<String> secondaryTargetName;

        @JsonProperty("SecondaryTargetIPAddress")
        private Ref<String> secondaryTargetIpAddress;

        @JsonProperty("SecondaryTargetTCPPort")
        private Ref<Integer> secondaryTargetTpcPort;

        @JsonProperty("SecondaryLUN")
        private Ref<Integer> secondaryLun;

        @JsonProperty("SecondaryVLANEnable")
        private Ref<Boolean> secondaryVlanEnable;

        @JsonProperty("SecondaryVLANId")
        private Ref<Integer> secondaryVlanId;

        @JsonProperty("SecondaryDNS")
        private Ref<String> secondaryDns;

        @JsonProperty("IPMaskDNSViaDHCP")
        private Ref<Boolean> ipMaskDnsViaDhcp;

        @JsonProperty("RouterAdvertisementEnabled")
        private Ref<Boolean> routerAdvertisementEnabled;

        @JsonProperty("AuthenticationMethod")
        private Ref<AuthenticationMethod> authenticationMethod;

        @JsonProperty("CHAPUsername")
        private Ref<String> chapUsername;

        @JsonProperty("CHAPSecret")
        private Ref<String> chapSecret;

        @JsonProperty("MutualCHAPUsername")
        private Ref<String> mutualChapUsername;

        @JsonProperty("MutualCHAPSecret")
        private Ref<String> mutualChapSecret;

        @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
        public IscsiBoot(NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition iscsiBootDefinition) {
            if (iscsiBootDefinition == null) {
                return;
            }

            iscsiBootDefinition.getIpAddressType().ifAssigned(ipAddressType1 -> this.ipAddressType = iscsiBootDefinition.getIpAddressType());
            iscsiBootDefinition.getInitiatorIpAddress()
                .ifAssigned(initiatorIpAddress -> this.initiatorIpAddress = iscsiBootDefinition.getInitiatorIpAddress());
            iscsiBootDefinition.getInitiatorName().ifAssigned(initiatorName -> this.initiatorName = iscsiBootDefinition.getInitiatorName());
            iscsiBootDefinition.getInitiatorDefaultGateway()
                .ifAssigned(initiatorDefaultGateway -> this.initiatorDefaultGateway = iscsiBootDefinition.getInitiatorDefaultGateway());
            iscsiBootDefinition.getInitiatorNetmask().ifAssigned(initiatorNetmask -> this.initiatorNetmask = iscsiBootDefinition.getInitiatorNetmask());
            iscsiBootDefinition.getTargetInfoViaDhcp().ifAssigned(targetInfoViaDhcp -> this.targetInfoViaDhcp = iscsiBootDefinition.getTargetInfoViaDhcp());
            iscsiBootDefinition.getPrimaryTargetName().ifAssigned(primaryTargetName -> this.primaryTargetName = iscsiBootDefinition.getPrimaryTargetName());
            iscsiBootDefinition.getPrimaryTargetIpAddress()
                .ifAssigned(primaryTargetIpAddress -> this.primaryTargetIpAddress = iscsiBootDefinition.getPrimaryTargetIpAddress());
            iscsiBootDefinition.getPrimaryTargetTcpPort()
                .ifAssigned(primaryTargetTcpPort -> this.primaryTargetTcpPort = iscsiBootDefinition.getPrimaryTargetTcpPort());
            iscsiBootDefinition.getPrimaryLun().ifAssigned(primaryLun -> this.primaryLun = iscsiBootDefinition.getPrimaryLun());
            iscsiBootDefinition.getPrimaryVlanEnable().ifAssigned(primaryVlanEnable -> this.primaryVlanEnable = iscsiBootDefinition.getPrimaryVlanEnable());
            iscsiBootDefinition.getPrimaryVlanId().ifAssigned(primaryVlanId -> this.primaryVlanId = iscsiBootDefinition.getPrimaryVlanId());
            iscsiBootDefinition.getPrimaryDns().ifAssigned(primaryDns -> this.primaryDns = iscsiBootDefinition.getPrimaryDns());
            iscsiBootDefinition.getSecondaryTargetName()
                .ifAssigned(secondaryTargetName -> this.secondaryTargetName = iscsiBootDefinition.getSecondaryTargetName());
            iscsiBootDefinition.getSecondaryTargetIpAddress()
                .ifAssigned(secondaryTargetIpAddress -> this.secondaryTargetIpAddress = iscsiBootDefinition.getSecondaryTargetIpAddress());
            iscsiBootDefinition.getSecondaryTargetTpcPort()
                .ifAssigned(secondaryTargetTpcPort -> this.secondaryTargetTpcPort = iscsiBootDefinition.getSecondaryTargetTpcPort());
            iscsiBootDefinition.getSecondaryLun().ifAssigned(secondaryLun -> this.secondaryLun = iscsiBootDefinition.getSecondaryLun());
            iscsiBootDefinition.getSecondaryVlanEnable()
                .ifAssigned(secondaryVlanEnable -> this.secondaryVlanEnable = iscsiBootDefinition.getSecondaryVlanEnable());
            iscsiBootDefinition.getSecondaryVlanId().ifAssigned(secondaryVlanId -> this.secondaryVlanId = iscsiBootDefinition.getSecondaryVlanId());
            iscsiBootDefinition.getSecondaryDns().ifAssigned(secondaryDns -> this.secondaryDns = iscsiBootDefinition.getSecondaryDns());
            iscsiBootDefinition.getIpMaskDnsViaDhcp().ifAssigned(ipMaskDnsViaDhcp -> this.ipMaskDnsViaDhcp = iscsiBootDefinition.getIpMaskDnsViaDhcp());
            iscsiBootDefinition.getRouterAdvertisementEnabled()
                .ifAssigned(routerAdvertisementEnabled -> this.routerAdvertisementEnabled = iscsiBootDefinition.getRouterAdvertisementEnabled());
            iscsiBootDefinition.getAuthenticationMethod()
                .ifAssigned(authenticationMethod -> this.authenticationMethod = iscsiBootDefinition.getAuthenticationMethod());
            iscsiBootDefinition.getChapUsername().ifAssigned(chapUsername -> this.chapUsername = iscsiBootDefinition.getChapUsername());
            iscsiBootDefinition.getChapSecret().ifAssigned(chapSecret -> this.chapSecret = iscsiBootDefinition.getChapSecret());
            iscsiBootDefinition.getMutualChapUsername()
                .ifAssigned(mutualChapUsername -> this.mutualChapUsername = iscsiBootDefinition.getMutualChapUsername());
            iscsiBootDefinition.getMutualChapSecret().ifAssigned(mutualChapSecret -> this.mutualChapSecret = iscsiBootDefinition.getMutualChapSecret());
        }
    }
}
