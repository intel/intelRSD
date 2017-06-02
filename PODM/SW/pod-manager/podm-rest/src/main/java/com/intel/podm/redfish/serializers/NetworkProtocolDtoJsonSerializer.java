/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.dto.redfish.NetworkProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.ProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.SimpleServiceDiscoveryProtocolDto;
import com.intel.podm.redfish.json.templates.NetworkProtocolJson;
import com.intel.podm.redfish.json.templates.NetworkProtocolJson.ProtocolJson;
import com.intel.podm.redfish.json.templates.NetworkProtocolJson.SimpleServiceDiscoveryProtocolJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class NetworkProtocolDtoJsonSerializer extends BaseDtoJsonSerializer<NetworkProtocolDto> {
    public NetworkProtocolDtoJsonSerializer() {
        super(NetworkProtocolDto.class);
    }

    @Override
    protected NetworkProtocolJson translate(NetworkProtocolDto dto) {
        NetworkProtocolJson networkProtocolJson = new NetworkProtocolJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        networkProtocolJson.oDataId = oDataId;
        networkProtocolJson.oDataContext = getContextFromId(oDataId);

        networkProtocolJson.id = dto.getId();
        networkProtocolJson.name = dto.getName();
        networkProtocolJson.description = dto.getDescription();
        networkProtocolJson.status = dto.getStatus();
        networkProtocolJson.hostName = dto.getHostName();
        networkProtocolJson.fqdn = dto.getFqdn();

        fillProtocols(networkProtocolJson, dto);

        return networkProtocolJson;
    }

    private void fillProtocols(NetworkProtocolJson networkProtocolJson, NetworkProtocolDto dto) {
        networkProtocolJson.http = mapToProtocol(dto.getHttp());
        networkProtocolJson.https = mapToProtocol(dto.getHttps());
        networkProtocolJson.ipmi = mapToProtocol(dto.getIpmi());
        networkProtocolJson.ssh = mapToProtocol(dto.getSsh());
        networkProtocolJson.snmp = mapToProtocol(dto.getSnmp());
        networkProtocolJson.virtualMedia = mapToProtocol(dto.getVirtualMedia());
        networkProtocolJson.ssdp = mapToSimpleServiceDiscoveryProtocol(dto.getSsdp());
        networkProtocolJson.telnet = mapToProtocol(dto.getTelnet());
        networkProtocolJson.kvmip = mapToProtocol(dto.getKvmip());
    }

    private ProtocolJson mapToProtocol(ProtocolDto protocolDto) {
        ProtocolJson protocolJson = new ProtocolJson();
        protocolJson.protocolEnabled = protocolDto.isProtocolEnabled();
        protocolJson.port = protocolDto.getPort();
        return protocolJson;
    }

    private SimpleServiceDiscoveryProtocolJson mapToSimpleServiceDiscoveryProtocol(SimpleServiceDiscoveryProtocolDto simpleServiceDiscoveryProtocolDto) {
        SimpleServiceDiscoveryProtocolJson protocolJson = new SimpleServiceDiscoveryProtocolJson();
        protocolJson.protocolEnabled = simpleServiceDiscoveryProtocolDto.isProtocolEnabled();
        protocolJson.port = simpleServiceDiscoveryProtocolDto.getPort();
        protocolJson.notifyMulticastIntervalSeconds = simpleServiceDiscoveryProtocolDto.getNotifyMulticastIntervalSeconds();
        protocolJson.notifyTtl = simpleServiceDiscoveryProtocolDto.getNotifyTtl();
        protocolJson.notifyIpV6Scope = simpleServiceDiscoveryProtocolDto.getNotifyIpV6Scope();
        return protocolJson;
    }
}
