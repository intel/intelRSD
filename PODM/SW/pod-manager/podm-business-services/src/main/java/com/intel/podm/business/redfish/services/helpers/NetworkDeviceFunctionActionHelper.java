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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishNetworkDeviceFunction;

public class NetworkDeviceFunctionActionHelper {

    public NetworkDeviceFunctionUpdateDefinition networkDeviceFunctionDefinition(RedfishNetworkDeviceFunction networkDeviceFunctionModification) {
        NetworkDeviceFunctionUpdateDefinition networkDeviceFunctionUpdateDefinition = new NetworkDeviceFunctionUpdateDefinition();
        networkDeviceFunctionUpdateDefinition.setDeviceEnabled(networkDeviceFunctionModification.getDeviceEnabled());
        networkDeviceFunctionModification.getIscsiBoot()
            .ifAssigned(iscsiBoot -> networkDeviceFunctionUpdateDefinition.setIscsiBoot(iscsiBootDefinition(iscsiBoot)));
        networkDeviceFunctionModification.getEthernet()
            .ifAssigned(ethernet -> networkDeviceFunctionUpdateDefinition.setEthernet(ethernetDefinition(ethernet)));
        return networkDeviceFunctionUpdateDefinition;
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    private Ref<NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition> iscsiBootDefinition(RedfishNetworkDeviceFunction.IscsiBoot iscsiBoot) {

        Ref<NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition> iscsiBootDefinition = Ref.of(null);

        if (iscsiBoot != null) {
            NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition iscsiBootDef = new NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition();

            iscsiBootDef.setAuthenticationMethod(iscsiBoot.getAuthenticationMethod());
            iscsiBootDef.setChapSecret(iscsiBoot.getChapSecret());
            iscsiBootDef.setChapUsername(iscsiBoot.getChapUsername());
            iscsiBootDef.setInitiatorDefaultGateway(iscsiBoot.getInitiatorDefaultGateway());
            iscsiBootDef.setInitiatorIpAddress(iscsiBoot.getInitiatorIpAddress());
            iscsiBootDef.setInitiatorName(iscsiBoot.getInitiatorName());
            iscsiBootDef.setInitiatorNetmask(iscsiBoot.getInitiatorNetmask());
            iscsiBootDef.setIpAddressType(iscsiBoot.getIpAddressType());
            iscsiBootDef.setIpMaskDnsViaDhcp(iscsiBoot.getIpMaskDnsViaDhcp());
            iscsiBootDef.setMutualChapSecret(iscsiBoot.getMutualChapSecret());
            iscsiBootDef.setMutualChapUsername(iscsiBoot.getMutualChapUsername());
            iscsiBootDef.setPrimaryDns(iscsiBoot.getPrimaryDns());
            iscsiBootDef.setPrimaryLun(iscsiBoot.getPrimaryLun());
            iscsiBootDef.setPrimaryTargetIpAddress(iscsiBoot.getPrimaryTargetIpAddress());
            iscsiBootDef.setPrimaryTargetName(iscsiBoot.getPrimaryTargetName());
            iscsiBootDef.setPrimaryTargetTcpPort(iscsiBoot.getPrimaryTargetTcpPort());
            iscsiBootDef.setPrimaryVlanEnable(iscsiBoot.getPrimaryVlanEnable());
            iscsiBootDef.setPrimaryVlanId(iscsiBoot.getPrimaryVlanId());
            iscsiBootDef.setRouterAdvertisementEnabled(iscsiBoot.getRouterAdvertisementEnabled());
            iscsiBootDef.setSecondaryDns(iscsiBoot.getSecondaryDns());
            iscsiBootDef.setSecondaryLun(iscsiBoot.getSecondaryLun());
            iscsiBootDef.setSecondaryTargetIpAddress(iscsiBoot.getSecondaryTargetIpAddress());
            iscsiBootDef.setSecondaryTargetName(iscsiBoot.getSecondaryTargetName());
            iscsiBootDef.setSecondaryTargetTpcPort(iscsiBoot.getSecondaryTargetTcpPort());
            iscsiBootDef.setSecondaryVlanEnable(iscsiBoot.getSecondaryVlanEnable());
            iscsiBootDef.setSecondaryVlanId(iscsiBoot.getSecondaryVlanId());
            iscsiBootDef.setTargetInfoViaDhcp(iscsiBoot.getTargetInfoViaDhcp());
            iscsiBootDefinition = Ref.of(iscsiBootDef);
        }
        return iscsiBootDefinition;
    }

    private Ref<NetworkDeviceFunctionUpdateDefinition.EthernetDefinition> ethernetDefinition(RedfishNetworkDeviceFunction.Ethernet ethernet) {

        Ref<NetworkDeviceFunctionUpdateDefinition.EthernetDefinition> ethernetDefinition = Ref.of(null);

        if (ethernet != null) {
            NetworkDeviceFunctionUpdateDefinition.EthernetDefinition ethernetDef = new NetworkDeviceFunctionUpdateDefinition.EthernetDefinition();
            ethernetDef.setMacAddress(ethernet.getMacAddress());
            ethernetDefinition = Ref.of(ethernetDef);
        }
        return ethernetDefinition;
    }
}

