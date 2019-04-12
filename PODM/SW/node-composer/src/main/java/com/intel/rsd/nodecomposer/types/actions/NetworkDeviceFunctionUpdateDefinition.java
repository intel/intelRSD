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

package com.intel.rsd.nodecomposer.types.actions;

import com.intel.rsd.nodecomposer.types.AuthenticationMethod;
import com.intel.rsd.nodecomposer.types.IpAddressType;
import com.intel.rsd.nodecomposer.types.Ref;

import static com.intel.rsd.nodecomposer.types.Ref.unassigned;

public final class NetworkDeviceFunctionUpdateDefinition {

    private Ref<Boolean> deviceEnabled = unassigned();
    private Ref<IscsiBootDefinition> iscsiBoot = unassigned();
    private Ref<EthernetDefinition> ethernet = unassigned();

    public Ref<Boolean> getDeviceEnabled() {
        return deviceEnabled;
    }

    public void setDeviceEnabled(Ref<Boolean> deviceEnabled) {
        this.deviceEnabled = deviceEnabled;
    }

    public Ref<IscsiBootDefinition> getIscsiBoot() {
        return iscsiBoot;
    }

    public void setIscsiBoot(Ref<IscsiBootDefinition> iscsiBoot) {
        this.iscsiBoot = iscsiBoot;
    }

    public Ref<EthernetDefinition> getEthernet() {
        return ethernet;
    }

    public void setEthernet(Ref<EthernetDefinition> ethernet) {
        this.ethernet = ethernet;
    }

    public static class EthernetDefinition {
        private Ref<String> macAddress;

        public Ref<String> getMacAddress() {
            return macAddress;
        }

        public void setMacAddress(Ref<String> macAddress) {
            this.macAddress = macAddress;
        }
    }

    @SuppressWarnings({"checkstyle:MethodCount"})
    public static class IscsiBootDefinition {
        private Ref<IpAddressType> ipAddressType = unassigned();
        private Ref<String> initiatorIpAddress = unassigned();
        private Ref<String> initiatorName = unassigned();
        private Ref<String> initiatorDefaultGateway = unassigned();
        private Ref<String> initiatorNetmask = unassigned();
        private Ref<Boolean> targetInfoViaDhcp = unassigned();
        private Ref<String> primaryTargetName = unassigned();
        private Ref<String> primaryTargetIpAddress = unassigned();
        private Ref<Integer> primaryTargetTcpPort = unassigned();
        private Ref<Integer> primaryLun = unassigned();
        private Ref<Boolean> primaryVlanEnable = unassigned();
        private Ref<Integer> primaryVlanId = unassigned();
        private Ref<String> primaryDns = unassigned();
        private Ref<String> secondaryTargetName = unassigned();
        private Ref<String> secondaryTargetIpAddress = unassigned();
        private Ref<Integer> secondaryTargetTpcPort = unassigned();
        private Ref<Integer> secondaryLun = unassigned();
        private Ref<Boolean> secondaryVlanEnable = unassigned();
        private Ref<Integer> secondaryVlanId = unassigned();
        private Ref<String> secondaryDns = unassigned();
        private Ref<Boolean> ipMaskDnsViaDhcp = unassigned();
        private Ref<Boolean> routerAdvertisementEnabled = unassigned();
        private Ref<AuthenticationMethod> authenticationMethod = unassigned();
        private Ref<String> chapUsername = unassigned();
        private Ref<String> chapSecret = unassigned();
        private Ref<String> mutualChapUsername = unassigned();
        private Ref<String> mutualChapSecret = unassigned();

        public Ref<IpAddressType> getIpAddressType() {
            return ipAddressType;
        }

        public void setIpAddressType(Ref<IpAddressType> ipAddressType) {
            this.ipAddressType = ipAddressType;
        }

        public Ref<String> getInitiatorIpAddress() {
            return initiatorIpAddress;
        }

        public void setInitiatorIpAddress(Ref<String> initiatorIpAddress) {
            this.initiatorIpAddress = initiatorIpAddress;
        }

        public Ref<String> getInitiatorName() {
            return initiatorName;
        }

        public void setInitiatorName(Ref<String> initiatorName) {
            this.initiatorName = initiatorName;
        }

        public Ref<String> getInitiatorDefaultGateway() {
            return initiatorDefaultGateway;
        }

        public void setInitiatorDefaultGateway(Ref<String> initiatorDefaultGateway) {
            this.initiatorDefaultGateway = initiatorDefaultGateway;
        }

        public Ref<String> getInitiatorNetmask() {
            return initiatorNetmask;
        }

        public void setInitiatorNetmask(Ref<String> initiatorNetmask) {
            this.initiatorNetmask = initiatorNetmask;
        }

        public Ref<Boolean> getTargetInfoViaDhcp() {
            return targetInfoViaDhcp;
        }

        public void setTargetInfoViaDhcp(Ref<Boolean> targetInfoViaDhcp) {
            this.targetInfoViaDhcp = targetInfoViaDhcp;
        }

        public Ref<String> getPrimaryTargetName() {
            return primaryTargetName;
        }

        public void setPrimaryTargetName(Ref<String> primaryTargetName) {
            this.primaryTargetName = primaryTargetName;
        }

        public Ref<String> getPrimaryTargetIpAddress() {
            return primaryTargetIpAddress;
        }

        public void setPrimaryTargetIpAddress(Ref<String> primaryTargetIpAddress) {
            this.primaryTargetIpAddress = primaryTargetIpAddress;
        }

        public Ref<Integer> getPrimaryTargetTcpPort() {
            return primaryTargetTcpPort;
        }

        public void setPrimaryTargetTcpPort(Ref<Integer> primaryTargetTcpPort) {
            this.primaryTargetTcpPort = primaryTargetTcpPort;
        }

        public Ref<Integer> getPrimaryLun() {
            return primaryLun;
        }

        public void setPrimaryLun(Ref<Integer> primaryLun) {
            this.primaryLun = primaryLun;
        }

        public Ref<Boolean> getPrimaryVlanEnable() {
            return primaryVlanEnable;
        }

        public void setPrimaryVlanEnable(Ref<Boolean> primaryVlanEnable) {
            this.primaryVlanEnable = primaryVlanEnable;
        }

        public Ref<Integer> getPrimaryVlanId() {
            return primaryVlanId;
        }

        public void setPrimaryVlanId(Ref<Integer> primaryVlanId) {
            this.primaryVlanId = primaryVlanId;
        }

        public Ref<String> getPrimaryDns() {
            return primaryDns;
        }

        public void setPrimaryDns(Ref<String> primaryDns) {
            this.primaryDns = primaryDns;
        }

        public Ref<String> getSecondaryTargetName() {
            return secondaryTargetName;
        }

        public void setSecondaryTargetName(Ref<String> secondaryTargetName) {
            this.secondaryTargetName = secondaryTargetName;
        }

        public Ref<String> getSecondaryTargetIpAddress() {
            return secondaryTargetIpAddress;
        }

        public void setSecondaryTargetIpAddress(Ref<String> secondaryTargetIpAddress) {
            this.secondaryTargetIpAddress = secondaryTargetIpAddress;
        }

        public Ref<Integer> getSecondaryTargetTpcPort() {
            return secondaryTargetTpcPort;
        }

        public void setSecondaryTargetTpcPort(Ref<Integer> secondaryTargetTpcPort) {
            this.secondaryTargetTpcPort = secondaryTargetTpcPort;
        }

        public Ref<Integer> getSecondaryLun() {
            return secondaryLun;
        }

        public void setSecondaryLun(Ref<Integer> secondaryLun) {
            this.secondaryLun = secondaryLun;
        }

        public Ref<Boolean> getSecondaryVlanEnable() {
            return secondaryVlanEnable;
        }

        public void setSecondaryVlanEnable(Ref<Boolean> secondaryVlanEnable) {
            this.secondaryVlanEnable = secondaryVlanEnable;
        }

        public Ref<Integer> getSecondaryVlanId() {
            return secondaryVlanId;
        }

        public void setSecondaryVlanId(Ref<Integer> secondaryVlanId) {
            this.secondaryVlanId = secondaryVlanId;
        }

        public Ref<String> getSecondaryDns() {
            return secondaryDns;
        }

        public void setSecondaryDns(Ref<String> secondaryDns) {
            this.secondaryDns = secondaryDns;
        }

        public Ref<Boolean> getIpMaskDnsViaDhcp() {
            return ipMaskDnsViaDhcp;
        }

        public void setIpMaskDnsViaDhcp(Ref<Boolean> ipMaskDnsViaDhcp) {
            this.ipMaskDnsViaDhcp = ipMaskDnsViaDhcp;
        }

        public Ref<Boolean> getRouterAdvertisementEnabled() {
            return routerAdvertisementEnabled;
        }

        public void setRouterAdvertisementEnabled(Ref<Boolean> routerAdvertisementEnabled) {
            this.routerAdvertisementEnabled = routerAdvertisementEnabled;
        }

        public Ref<AuthenticationMethod> getAuthenticationMethod() {
            return authenticationMethod;
        }

        public void setAuthenticationMethod(Ref<AuthenticationMethod> authenticationMethod) {
            this.authenticationMethod = authenticationMethod;
        }

        public Ref<String> getChapUsername() {
            return chapUsername;
        }

        public void setChapUsername(Ref<String> chapUsername) {
            this.chapUsername = chapUsername;
        }

        public Ref<String> getChapSecret() {
            return chapSecret;
        }

        public void setChapSecret(Ref<String> chapSecret) {
            this.chapSecret = chapSecret;
        }

        public Ref<String> getMutualChapUsername() {
            return mutualChapUsername;
        }

        public void setMutualChapUsername(Ref<String> mutualChapUsername) {
            this.mutualChapUsername = mutualChapUsername;
        }

        public Ref<String> getMutualChapSecret() {
            return mutualChapSecret;
        }

        public void setMutualChapSecret(Ref<String> mutualChapSecret) {
            this.mutualChapSecret = mutualChapSecret;
        }
    }
}
