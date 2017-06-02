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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.EthernetDto;
import com.intel.podm.business.dto.redfish.IscsiBootDto;
import com.intel.podm.business.dto.redfish.NetworkDeviceFunctionDto;
import com.intel.podm.redfish.json.templates.BaseJson;
import com.intel.podm.redfish.json.templates.NetworkDeviceFunctionJson;
import com.intel.podm.redfish.json.templates.NetworkDeviceFunctionJson.EthernetJson;
import com.intel.podm.redfish.json.templates.NetworkDeviceFunctionJson.IscsiBootJson;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class NetworkDeviceFunctionDtoJsonSerializer extends BaseDtoJsonSerializer<NetworkDeviceFunctionDto> {

    protected NetworkDeviceFunctionDtoJsonSerializer() {
        super(NetworkDeviceFunctionDto.class);
    }

    @Override
    protected BaseJson translate(NetworkDeviceFunctionDto dto) {
        NetworkDeviceFunctionJson networkDeviceFunctionJson = new NetworkDeviceFunctionJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        networkDeviceFunctionJson.oDataId = oDataId;
        networkDeviceFunctionJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        networkDeviceFunctionJson.id = dto.getId();
        networkDeviceFunctionJson.name = dto.getName();
        networkDeviceFunctionJson.description = dto.getDescription();
        networkDeviceFunctionJson.status = dto.getStatus();
        networkDeviceFunctionJson.deviceEnabled = dto.getDeviceEnabled();
        fillEthernet(networkDeviceFunctionJson.ethernet, dto.getEthernet());
        fillIscsiBoot(networkDeviceFunctionJson.iscsiBoot, dto.getIscsiBoot());
        return networkDeviceFunctionJson;
    }

    private void fillEthernet(EthernetJson ethernetJson, EthernetDto ethernet) {
        if (ethernet == null) {
            return;
        }

        ethernetJson.macAddress = ethernet.getMacAddress();
    }

    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    private void fillIscsiBoot(IscsiBootJson iscsiBootJson, IscsiBootDto iscsiBoot) {
        if (iscsiBoot == null) {
            return;
        }

        iscsiBootJson.ipAddressType = iscsiBoot.getIpAddressType();
        iscsiBootJson.initiatorIpAddress = iscsiBoot.getInitiatorIpAddress();
        iscsiBootJson.initiatorName = iscsiBoot.getInitiatorName();
        iscsiBootJson.initiatorDefaultGateway = iscsiBoot.getInitiatorDefaultGateway();
        iscsiBootJson.initiatorNetmask = iscsiBoot.getInitiatorNetmask();
        iscsiBootJson.targetInfoViaDhcp = iscsiBoot.getTargetInfoViaDhcp();
        iscsiBootJson.primaryTargetName = iscsiBoot.getPrimaryTargetName();
        iscsiBootJson.primaryTargetIpAddress = iscsiBoot.getPrimaryTargetIpAddress();
        iscsiBootJson.primaryTargetTcpPort = iscsiBoot.getPrimaryTargetTcpPort();
        iscsiBootJson.primaryLun = iscsiBoot.getPrimaryLun();
        iscsiBootJson.primaryVlanEnable = iscsiBoot.getPrimaryVlanEnable();
        iscsiBootJson.primaryVlanId = iscsiBoot.getPrimaryVlanId();
        iscsiBootJson.primaryDns = iscsiBoot.getPrimaryDns();
        iscsiBootJson.secondaryTargetName = iscsiBoot.getSecondaryTargetName();
        iscsiBootJson.secondaryTargetIpAddress = iscsiBoot.getSecondaryTargetIpAddress();
        iscsiBootJson.secondaryTargetTcpPort = iscsiBoot.getSecondaryTargetTcpPort();
        iscsiBootJson.secondaryLun = iscsiBoot.getSecondaryLun();
        iscsiBootJson.secondaryVlanEnable = iscsiBoot.getSecondaryVlanEnable();
        iscsiBootJson.secondaryVlanId = iscsiBoot.getSecondaryVlanId();
        iscsiBootJson.secondaryDns = iscsiBoot.getSecondaryDns();
        iscsiBootJson.ipMaskDnsViaDhcp = iscsiBoot.getIpMaskDnsViaDhcp();
        iscsiBootJson.routerAdvertisementEnabled = iscsiBoot.getRouterAdvertisementEnabled();
        iscsiBootJson.authenticationMethod = iscsiBoot.getAuthenticationMethod();
        iscsiBootJson.chapUsername = iscsiBoot.getChapUsername();
        iscsiBootJson.chapSecret = iscsiBoot.getChapSecret();
        iscsiBootJson.mutualChapUsername = iscsiBoot.getMutualChapUsername();
        iscsiBootJson.mutualChapSecret = iscsiBoot.getMutualChapSecret();
    }
}
