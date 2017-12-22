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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishNetworkDeviceFunction;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import static com.intel.podm.common.types.Ref.unassigned;

public final class NetworkDeviceFunctionPartialRepresentation implements RedfishNetworkDeviceFunction {

    @JsonProperty("DeviceEnabled")
    private Ref<Boolean> deviceEnabled = unassigned();

    @JsonProperty("iSCSIBoot")
    private Ref<IscsiBoot> iscsiBoot = unassigned();

    @JsonProperty("Ethernet")
    private Ref<Ethernet> ethernet = unassigned();

    @Override
    public Ref<Boolean> getDeviceEnabled() {
        return deviceEnabled;
    }

    @Override
    public Ref<RedfishNetworkDeviceFunction.IscsiBoot> getIscsiBoot() {
        return (Ref) iscsiBoot;
    }

    @Override
    public Ref<RedfishNetworkDeviceFunction.Ethernet> getEthernet() {
        return (Ref) ethernet;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (o == null || getClass() != o.getClass()) {
            return false;
        }
        NetworkDeviceFunctionPartialRepresentation that = (NetworkDeviceFunctionPartialRepresentation) o;
        return new EqualsBuilder()
            .append(deviceEnabled, that.deviceEnabled)
            .append(iscsiBoot, that.iscsiBoot)
            .append(ethernet, that.ethernet)
            .isEquals();
    }

    @Override
    public int hashCode() {
        return new HashCodeBuilder()
            .append(deviceEnabled)
            .append(iscsiBoot)
            .append(ethernet)
            .toHashCode();
    }

    public static class Ethernet implements RedfishNetworkDeviceFunction.Ethernet {
        @JsonProperty("MACAddress")
        private Ref<String> macAddress = unassigned();

        @Override
        public Ref<String> getMacAddress() {
            return macAddress;
        }
    }

    @SuppressWarnings({"checkstyle:MethodCount"})
    public static class IscsiBoot implements RedfishNetworkDeviceFunction.IscsiBoot {
        @JsonProperty("IPAddressType")
        private Ref<IpAddressType> ipAddressType = unassigned();

        @JsonProperty("InitiatorIPAddress")
        private Ref<String> initiatorIpAddress = unassigned();

        @JsonProperty("InitiatorName")
        private Ref<String> initiatorName = unassigned();

        @JsonProperty("InitiatorDefaultGateway")
        private Ref<String> initiatorDefaultGateway = unassigned();

        @JsonProperty("InitiatorNetmask")
        private Ref<String> initiatorNetmask = unassigned();

        @JsonProperty("TargetInfoViaDHCP")
        private Ref<Boolean> targetInfoViaDhcp = unassigned();

        @JsonProperty("PrimaryTargetName")
        private Ref<String> primaryTargetName = unassigned();

        @JsonProperty("PrimaryTargetIPAddress")
        private Ref<String> primaryTargetIpAddress = unassigned();

        @JsonProperty("PrimaryTargetTCPPort")
        private Ref<Integer> primaryTargetTcpPort = unassigned();

        @JsonProperty("PrimaryLUN")
        private Ref<Integer> primaryLun = unassigned();

        @JsonProperty("PrimaryVLANEnable")
        private Ref<Boolean> primaryVlanEnable = unassigned();

        @JsonProperty("PrimaryVLANId")
        private Ref<Integer> primaryVlanId = unassigned();

        @JsonProperty("PrimaryDNS")
        private Ref<String> primaryDns = unassigned();

        @JsonProperty("SecondaryTargetName")
        private Ref<String> secondaryTargetName = unassigned();

        @JsonProperty("SecondaryTargetIPAddress")
        private Ref<String> secondaryTargetIpAddress = unassigned();

        @JsonProperty("SecondaryTargetTCPPort")
        private Ref<Integer> secondaryTargetTpcPort = unassigned();

        @JsonProperty("SecondaryLUN")
        private Ref<Integer> secondaryLun = unassigned();

        @JsonProperty("SecondaryVLANEnable")
        private Ref<Boolean> secondaryVlanEnable = unassigned();

        @JsonProperty("SecondaryVLANId")
        private Ref<Integer> secondaryVlanId = unassigned();

        @JsonProperty("SecondaryDNS")
        private Ref<String> secondaryDns = unassigned();

        @JsonProperty("IPMaskDNSViaDHCP")
        private Ref<Boolean> ipMaskDnsViaDhcp = unassigned();

        @JsonProperty("RouterAdvertisementEnabled")
        private Ref<Boolean> routerAdvertisementEnabled = unassigned();

        @JsonProperty("AuthenticationMethod")
        private Ref<AuthenticationMethod> authenticationMethod = unassigned();

        @JsonProperty("CHAPUsername")
        private Ref<String> chapUsername = unassigned();

        @JsonProperty("CHAPSecret")
        private Ref<String> chapSecret = unassigned();

        @JsonProperty("MutualCHAPUsername")
        private Ref<String> mutualChapUsername = unassigned();

        @JsonProperty("MutualCHAPSecret")
        private Ref<String> mutualChapSecret = unassigned();

        @Override
        public Ref<IpAddressType> getIpAddressType() {
            return ipAddressType;
        }

        @Override
        public Ref<String> getInitiatorIpAddress() {
            return initiatorIpAddress;
        }

        @Override
        public Ref<String> getInitiatorName() {
            return initiatorName;
        }

        @Override
        public Ref<String> getInitiatorDefaultGateway() {
            return initiatorDefaultGateway;
        }

        @Override
        public Ref<String> getInitiatorNetmask() {
            return initiatorNetmask;
        }

        @Override
        public Ref<Boolean> getTargetInfoViaDhcp() {
            return targetInfoViaDhcp;
        }

        @Override
        public Ref<String> getPrimaryTargetName() {
            return primaryTargetName;
        }

        @Override
        public Ref<String> getPrimaryTargetIpAddress() {
            return primaryTargetIpAddress;
        }

        @Override
        public Ref<Integer> getPrimaryTargetTcpPort() {
            return primaryTargetTcpPort;
        }

        @Override
        public Ref<Integer> getPrimaryLun() {
            return primaryLun;
        }

        @Override
        public Ref<Boolean> getPrimaryVlanEnable() {
            return primaryVlanEnable;
        }

        @Override
        public Ref<Integer> getPrimaryVlanId() {
            return primaryVlanId;
        }

        @Override
        public Ref<String> getPrimaryDns() {
            return primaryDns;
        }

        @Override
        public Ref<String> getSecondaryTargetName() {
            return secondaryTargetName;
        }

        @Override
        public Ref<String> getSecondaryTargetIpAddress() {
            return secondaryTargetIpAddress;
        }

        @Override
        public Ref<Integer> getSecondaryTargetTcpPort() {
            return secondaryTargetTpcPort;
        }

        @Override
        public Ref<Integer> getSecondaryLun() {
            return secondaryLun;
        }

        @Override
        public Ref<Boolean> getSecondaryVlanEnable() {
            return secondaryVlanEnable;
        }

        @Override
        public Ref<Integer> getSecondaryVlanId() {
            return secondaryVlanId;
        }

        @Override
        public Ref<String> getSecondaryDns() {
            return secondaryDns;
        }

        @Override
        public Ref<Boolean> getIpMaskDnsViaDhcp() {
            return ipMaskDnsViaDhcp;
        }

        @Override
        public Ref<Boolean> getRouterAdvertisementEnabled() {
            return routerAdvertisementEnabled;
        }

        @Override
        public Ref<AuthenticationMethod> getAuthenticationMethod() {
            return authenticationMethod;
        }

        @Override
        public Ref<String> getChapUsername() {
            return chapUsername;
        }

        @Override
        public Ref<String> getChapSecret() {
            return chapSecret;
        }

        @Override
        public Ref<String> getMutualChapUsername() {
            return mutualChapUsername;
        }

        @Override
        public Ref<String> getMutualChapSecret() {
            return mutualChapSecret;
        }

        @Override
        @SuppressWarnings({"checkstyle:MethodLength"})
        public boolean equals(Object o) {
            if (this == o) {
                return true;
            }
            if (o == null || getClass() != o.getClass()) {
                return false;
            }
            IscsiBoot iscsiBoot = (IscsiBoot) o;
            return new EqualsBuilder()
                .append(ipAddressType, iscsiBoot.ipAddressType)
                .append(initiatorIpAddress, iscsiBoot.initiatorIpAddress)
                .append(initiatorName, iscsiBoot.initiatorName)
                .append(initiatorDefaultGateway, iscsiBoot.initiatorDefaultGateway)
                .append(initiatorNetmask, iscsiBoot.initiatorNetmask)
                .append(targetInfoViaDhcp, iscsiBoot.targetInfoViaDhcp)
                .append(primaryTargetName, iscsiBoot.primaryTargetName)
                .append(primaryTargetIpAddress, iscsiBoot.primaryTargetIpAddress)
                .append(primaryTargetTcpPort, iscsiBoot.primaryTargetTcpPort)
                .append(primaryLun, iscsiBoot.primaryLun)
                .append(primaryVlanEnable, iscsiBoot.primaryVlanEnable)
                .append(primaryVlanId, iscsiBoot.primaryVlanId)
                .append(primaryDns, iscsiBoot.primaryDns)
                .append(secondaryTargetName, iscsiBoot.secondaryTargetName)
                .append(secondaryTargetIpAddress, iscsiBoot.secondaryTargetIpAddress)
                .append(secondaryTargetTpcPort, iscsiBoot.secondaryTargetTpcPort)
                .append(secondaryLun, iscsiBoot.secondaryLun)
                .append(secondaryVlanEnable, iscsiBoot.secondaryVlanEnable)
                .append(secondaryVlanId, iscsiBoot.secondaryVlanId)
                .append(secondaryDns, iscsiBoot.secondaryDns)
                .append(ipMaskDnsViaDhcp, iscsiBoot.ipMaskDnsViaDhcp)
                .append(routerAdvertisementEnabled, iscsiBoot.routerAdvertisementEnabled)
                .append(authenticationMethod, iscsiBoot.authenticationMethod)
                .append(chapUsername, iscsiBoot.chapUsername)
                .append(chapSecret, iscsiBoot.chapSecret)
                .append(mutualChapUsername, iscsiBoot.mutualChapUsername)
                .append(mutualChapSecret, iscsiBoot.mutualChapSecret)
                .isEquals();
        }

        @Override
        @SuppressWarnings({"checkstyle:MethodLength"})
        public int hashCode() {
            return new HashCodeBuilder()
                .append(ipAddressType)
                .append(initiatorIpAddress)
                .append(initiatorName)
                .append(initiatorDefaultGateway)
                .append(initiatorNetmask)
                .append(targetInfoViaDhcp)
                .append(primaryTargetName)
                .append(primaryTargetIpAddress)
                .append(primaryTargetTcpPort)
                .append(primaryLun)
                .append(primaryVlanEnable)
                .append(primaryVlanId)
                .append(primaryDns)
                .append(secondaryTargetName)
                .append(secondaryTargetIpAddress)
                .append(secondaryTargetTpcPort)
                .append(secondaryLun)
                .append(secondaryVlanEnable)
                .append(secondaryVlanId)
                .append(secondaryDns)
                .append(ipMaskDnsViaDhcp)
                .append(routerAdvertisementEnabled)
                .append(authenticationMethod)
                .append(chapUsername)
                .append(chapSecret)
                .append(mutualChapUsername)
                .append(mutualChapSecret)
                .toHashCode();
        }
    }
}
