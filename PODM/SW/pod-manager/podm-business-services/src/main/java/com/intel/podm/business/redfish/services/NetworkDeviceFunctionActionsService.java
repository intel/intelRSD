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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.actions.NetworkDeviceFunctionInvoker;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.actions.NetworkDeviceFunctionUpdateDefinition;
import com.intel.podm.common.types.redfish.RedfishNetworkDeviceFunction;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class NetworkDeviceFunctionActionsService {

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private NetworkDeviceFunctionInvoker networkDeviceFunctionInvoker;

    @Transactional(REQUIRES_NEW)
    public void updateNetworkDeviceFunction(Context context, RedfishNetworkDeviceFunction representation)
        throws ContextResolvingException, EntityOperationException {
        NetworkDeviceFunction networkDeviceFunction = (NetworkDeviceFunction) traverser.traverse(context);
        networkDeviceFunctionInvoker.updateNetworkDeviceFunction(networkDeviceFunction, definitionFromRepresentation(representation));
    }

    private NetworkDeviceFunctionUpdateDefinition definitionFromRepresentation(RedfishNetworkDeviceFunction networkDeviceFunctionModification) {
        NetworkDeviceFunctionUpdateDefinition definition = new NetworkDeviceFunctionUpdateDefinition();
        definition.setDeviceEnabled(networkDeviceFunctionModification.getDeviceEnabled());
        networkDeviceFunctionModification.getIscsiBoot().ifAssigned(iscsiBoot -> definition.setIscsiBoot(createIscsiBootDefinition(iscsiBoot)));
        networkDeviceFunctionModification.getEthernet().ifAssigned(ethernet -> definition.setEthernet(createEthernetDefinition(ethernet)));
        return definition;
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    private Ref<NetworkDeviceFunctionUpdateDefinition.IscsiBootDefinition> createIscsiBootDefinition(RedfishNetworkDeviceFunction.IscsiBoot iscsiBoot) {

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

    private Ref<NetworkDeviceFunctionUpdateDefinition.EthernetDefinition> createEthernetDefinition(RedfishNetworkDeviceFunction.Ethernet ethernet) {

        Ref<NetworkDeviceFunctionUpdateDefinition.EthernetDefinition> ethernetDefinition = Ref.of(null);

        if (ethernet != null) {
            NetworkDeviceFunctionUpdateDefinition.EthernetDefinition ethernetDef = new NetworkDeviceFunctionUpdateDefinition.EthernetDefinition();
            ethernetDef.setMacAddress(ethernet.getMacAddress());
            ethernetDefinition = Ref.of(ethernetDef);
        }
        return ethernetDefinition;
    }
}
