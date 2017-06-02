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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EthernetDto;
import com.intel.podm.business.dto.redfish.IscsiBootDto;
import com.intel.podm.business.dto.redfish.NetworkDeviceFunctionDto;
import com.intel.podm.business.entities.redfish.NetworkDeviceFunction;
import com.intel.podm.business.entities.redfish.NetworkInterface;
import com.intel.podm.business.entities.redfish.embeddables.Ethernet;
import com.intel.podm.business.entities.redfish.embeddables.IscsiBoot;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.NETWORK_DEVICE_FUNTIONS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class NetworkDeviceFunctionServiceImpl implements ReaderService<NetworkDeviceFunctionDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        NetworkInterface networkInterface = (NetworkInterface) traverser.traverse(context);
        return new CollectionDto(NETWORK_DEVICE_FUNTIONS, getAsIdSet(networkInterface.getNetworkDeviceFunctions()));
    }

    @Override
    public NetworkDeviceFunctionDto getResource(Context context) throws ContextResolvingException {
        NetworkDeviceFunction networkDeviceFunction = (NetworkDeviceFunction) traverser.traverse(context);
        return NetworkDeviceFunctionDto.newBuilder()
            .id(networkDeviceFunction.getId().toString())
            .name(networkDeviceFunction.getName())
            .description(networkDeviceFunction.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(networkDeviceFunction.getService(), networkDeviceFunction.getUnknownOems()))
            .status(networkDeviceFunction.getStatus())
            .deviceEnabled(networkDeviceFunction.isDeviceEnabled())
            .ethernet(buildEthernet(networkDeviceFunction.getEthernet()))
            .iscsiBoot(buildIscsiBoot(networkDeviceFunction.getIscsiBoot()))
            .build();
    }

    private EthernetDto buildEthernet(Ethernet ethernet) {
        if (ethernet == null) {
            return null;
        }

        return EthernetDto.newBuilder()
            .macAddress(ethernet.getMacAddress())
            .build();
    }

    @SuppressWarnings({"checkstyle:MethodLength"})
    private IscsiBootDto buildIscsiBoot(IscsiBoot iscsiBoot) {
        if (iscsiBoot == null) {
            return null;
        }

        return IscsiBootDto.newBuilder()
            .ipAddressType(iscsiBoot.getIpAddressType())
            .initiatorIpAddress(iscsiBoot.getInitiatorIpAddress())
            .initiatorName(iscsiBoot.getInitiatorName())
            .initiatorDefaultGateway(iscsiBoot.getInitiatorDefaultGateway())
            .initiatorNetmask(iscsiBoot.getInitiatorNetmask())
            .targetInfoViaDhcp(iscsiBoot.getTargetInfoViaDhcp())
            .primaryTargetName(iscsiBoot.getPrimaryTargetName())
            .primaryTargetIpAddress(iscsiBoot.getPrimaryTargetIpAddress())
            .primaryTargetTcpPort(iscsiBoot.getPrimaryTargetTcpPort())
            .primaryLun(iscsiBoot.getPrimaryLun())
            .primaryVlanEnable(iscsiBoot.getPrimaryVlanEnable())
            .primaryVlanId(iscsiBoot.getPrimaryVlanId())
            .primaryDns(iscsiBoot.getPrimaryDns())
            .secondaryTargetName(iscsiBoot.getSecondaryTargetName())
            .secondaryTargetIpAddress(iscsiBoot.getSecondaryTargetIpAddress())
            .secondaryTargetTcpPort(iscsiBoot.getSecondaryTargetTcpPort())
            .secondaryLun(iscsiBoot.getSecondaryLun())
            .secondaryVlanEnable(iscsiBoot.getSecondaryVlanEnable())
            .secondaryVlanId(iscsiBoot.getSecondaryVlanId())
            .secondaryDns(iscsiBoot.getSecondaryDns())
            .ipMaskDnsViaDhcp(iscsiBoot.getIpMaskDnsViaDhcp())
            .routerAdvertisementEnabled(iscsiBoot.getRouterAdvertisementEnabled())
            .authenticationMethod(iscsiBoot.getAuthenticationMethod())
            .chapUsername(iscsiBoot.getChapUsername())
            .chapSecret(iscsiBoot.getChapSecret())
            .mutualChapUsername(iscsiBoot.getMutualChapUsername())
            .mutualChapSecret(iscsiBoot.getMutualChapSecret())
            .build();
    }
}
