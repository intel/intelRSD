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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.podm.common.types.AuthenticationMethod;
import com.intel.podm.common.types.IpAddressType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishNetworkDeviceFunction;
import org.apache.commons.lang3.builder.EqualsBuilder;
import org.apache.commons.lang3.builder.HashCodeBuilder;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.podm.common.types.Ref.unassigned;

public final class NetworkDeviceFunctionPartialRepresentation implements RedfishNetworkDeviceFunction {
    @JsonSetter(value = "DeviceEnabled", nulls = AS_EMPTY)
    private Ref<Boolean> deviceEnabled = unassigned();

    @JsonSetter(value = "iSCSIBoot", nulls = AS_EMPTY)
    private Ref<IscsiBoot> iscsiBoot = unassigned();

    @JsonSetter(value = "Ethernet", nulls = AS_EMPTY)
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
        @JsonSetter(value = "MACAddress", nulls = AS_EMPTY)
        private Ref<String> macAddress = unassigned();

        @Override
        public Ref<String> getMacAddress() {
            return macAddress;
        }
    }

    @SuppressWarnings({"checkstyle:MethodCount"})
    public static class IscsiBoot implements RedfishNetworkDeviceFunction.IscsiBoot {
        @JsonSetter(value = "IPAddressType", nulls = AS_EMPTY)
        private Ref<IpAddressType> ipAddressType = unassigned();

        @JsonSetter(value = "InitiatorIPAddress", nulls = AS_EMPTY)
        private Ref<String> initiatorIpAddress = unassigned();

        @JsonSetter(value = "InitiatorName", nulls = AS_EMPTY)
        private Ref<String> initiatorName = unassigned();

        @JsonSetter(value = "InitiatorDefaultGateway", nulls = AS_EMPTY)
        private Ref<String> initiatorDefaultGateway = unassigned();

        @JsonSetter(value = "InitiatorNetmask", nulls = AS_EMPTY)
        private Ref<String> initiatorNetmask = unassigned();

        @JsonSetter(value = "TargetInfoViaDHCP", nulls = AS_EMPTY)
        private Ref<Boolean> targetInfoViaDhcp = unassigned();

        @JsonSetter(value = "PrimaryTargetName", nulls = AS_EMPTY)
        private Ref<String> primaryTargetName = unassigned();

        @JsonSetter(value = "PrimaryTargetIPAddress", nulls = AS_EMPTY)
        private Ref<String> primaryTargetIpAddress = unassigned();

        @JsonSetter(value = "PrimaryTargetTCPPort", nulls = AS_EMPTY)
        private Ref<Integer> primaryTargetTcpPort = unassigned();

        @JsonSetter(value = "PrimaryLUN", nulls = AS_EMPTY)
        private Ref<Integer> primaryLun = unassigned();

        @JsonSetter(value = "PrimaryVLANEnable", nulls = AS_EMPTY)
        private Ref<Boolean> primaryVlanEnable = unassigned();

        @JsonSetter(value = "PrimaryVLANId", nulls = AS_EMPTY)
        private Ref<Integer> primaryVlanId = unassigned();

        @JsonSetter(value = "PrimaryDNS", nulls = AS_EMPTY)
        private Ref<String> primaryDns = unassigned();

        @JsonSetter(value = "SecondaryTargetName", nulls = AS_EMPTY)
        private Ref<String> secondaryTargetName = unassigned();

        @JsonSetter(value = "SecondaryTargetIPAddress", nulls = AS_EMPTY)
        private Ref<String> secondaryTargetIpAddress = unassigned();

        @JsonSetter(value = "SecondaryTargetTCPPort", nulls = AS_EMPTY)
        private Ref<Integer> secondaryTargetTpcPort = unassigned();

        @JsonSetter(value = "SecondaryLUN", nulls = AS_EMPTY)
        private Ref<Integer> secondaryLun = unassigned();

        @JsonSetter(value = "SecondaryVLANEnable", nulls = AS_EMPTY)
        private Ref<Boolean> secondaryVlanEnable = unassigned();

        @JsonSetter(value = "SecondaryVLANId", nulls = AS_EMPTY)
        private Ref<Integer> secondaryVlanId = unassigned();

        @JsonSetter(value = "SecondaryDNS", nulls = AS_EMPTY)
        private Ref<String> secondaryDns = unassigned();

        @JsonSetter(value = "IPMaskDNSViaDHCP", nulls = AS_EMPTY)
        private Ref<Boolean> ipMaskDnsViaDhcp = unassigned();

        @JsonSetter(value = "RouterAdvertisementEnabled", nulls = AS_EMPTY)
        private Ref<Boolean> routerAdvertisementEnabled = unassigned();

        @JsonSetter(value = "AuthenticationMethod", nulls = AS_EMPTY)
        private Ref<AuthenticationMethod> authenticationMethod = unassigned();

        @JsonSetter(value = "CHAPUsername", nulls = AS_EMPTY)
        private Ref<String> chapUsername = unassigned();

        @JsonSetter(value = "CHAPSecret", nulls = AS_EMPTY)
        private Ref<String> chapSecret = unassigned();

        @JsonSetter(value = "MutualCHAPUsername", nulls = AS_EMPTY)
        private Ref<String> mutualChapUsername = unassigned();

        @JsonSetter(value = "MutualCHAPSecret", nulls = AS_EMPTY)
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
