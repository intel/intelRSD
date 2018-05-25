/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.intel.podm.business.dto.NetworkProtocolDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.NEVER;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class NetworkProtocolReaderServiceImpl implements ReaderService<NetworkProtocolDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    @Transactional(NEVER)
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        throw new UnsupportedOperationException("NetworkProtocol is a singleton resource!");
    }

    @Override
    @Transactional(REQUIRED)
    public NetworkProtocolDto getResource(Context context) throws ContextResolvingException {
        Manager manager = (Manager) traverser.traverse(context);
        if (manager.getNetworkProtocol() == null) {
            throw new ContextResolvingException("NetworkProtocol Resource not found for specified Manager.", context, null);
        }
        return mapNetworkProtocol(manager.getNetworkProtocol());
    }

    private NetworkProtocolDto mapNetworkProtocol(NetworkProtocol networkProtocol) {
        NetworkProtocolDto dto = new NetworkProtocolDto();
        dto.setId(networkProtocol.getId().toString());
        dto.setName(networkProtocol.getName());
        dto.setDescription(networkProtocol.getDescription());
        dto.setUnknownOems(unknownOemTranslator.translateUnknownOemToDtos(networkProtocol.getService(), networkProtocol.getUnknownOems()));
        dto.setStatus(networkProtocol.getStatus());
        dto.setHostName(networkProtocol.getHostName());
        dto.setFqdn(networkProtocol.getFqdn());
        mapProtocols(networkProtocol, dto);
        return dto;
    }

    private void mapProtocols(NetworkProtocol networkProtocol, NetworkProtocolDto dto) {
        dto.setHttp(toProtocolDto(networkProtocol.getHttpProtocolEnabled(), networkProtocol.getHttpPort()));
        dto.setHttps(toProtocolDto(networkProtocol.getHttpsProtocolEnabled(), networkProtocol.getHttpsPort()));
        dto.setIpmi(toProtocolDto(networkProtocol.getIpmiProtocolEnabled(), networkProtocol.getIpmiPort()));
        dto.setSsh(toProtocolDto(networkProtocol.getSshProtocolEnabled(), networkProtocol.getSshPort()));
        dto.setSnmp(toProtocolDto(networkProtocol.getSnmpProtocolEnabled(), networkProtocol.getSnmpPort()));
        dto.setVirtualMedia(toProtocolDto(networkProtocol.getVirtualMediaProtocolEnabled(), networkProtocol.getVirtualMediaPort()));
        dto.setSsdp(getSsdpProtocolDto(networkProtocol));
        dto.setTelnet(toProtocolDto(networkProtocol.getTelnetProtocolEnabled(), networkProtocol.getTelnetPort()));
        dto.setKvmip(toProtocolDto(networkProtocol.getKvmIpProtocolEnabled(), networkProtocol.getKvmIpPort()));
    }

    private NetworkProtocolDto.SimpleServiceDiscoveryProtocolDto getSsdpProtocolDto(NetworkProtocol networkProtocol) {
        NetworkProtocolDto.SimpleServiceDiscoveryProtocolDto dto = new NetworkProtocolDto.SimpleServiceDiscoveryProtocolDto();
        dto.setProtocolEnabled(networkProtocol.getSsdpProtocolEnabled());
        dto.setPort(networkProtocol.getSsdpPort());
        dto.setNotifyMulticastIntervalSeconds(networkProtocol.getSsdpNotifyMulticastIntervalSeconds());
        dto.setNotifyTtl(networkProtocol.getSsdpNotifyTtl());
        dto.setNotifyIpV6Scope(networkProtocol.getSsdpNotifyIpV6Scope());
        return dto;
    }

    private NetworkProtocolDto.ProtocolDto toProtocolDto(Boolean protocolEnabled, Integer port) {
        NetworkProtocolDto.ProtocolDto dto = new NetworkProtocolDto.ProtocolDto();
        dto.setProtocolEnabled(protocolEnabled);
        dto.setPort(port);
        return dto;
    }
}
