/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.NetworkProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.ProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.SimpleServiceDiscoveryProtocolDto;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.NetworkProtocol;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.NetworkProtocolService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class NetworkProtocolServiceImpl implements NetworkProtocolService {

    @Inject
    DomainObjectTreeTraverser traverser;

    @Override
    public NetworkProtocolDto getNetworkProtocol(Context managerContext) throws EntityNotFoundException {
        Manager manager = (Manager) traverser.traverse(managerContext);
        NetworkProtocol networkProtocol = manager.getNetworkProtocol();
        if (networkProtocol == null) {
            throw new EntityNotFoundException("Network Protocol was not found for Manager");
        }
        return map(networkProtocol);
    }

    private NetworkProtocolDto map(NetworkProtocol networkProtocol) {
        return NetworkProtocolDto.newBuilder()
                .id(networkProtocol.getId())
                .name(networkProtocol.getName())
                .description(networkProtocol.getDescription())
                .status(networkProtocol.getStatus())
                .hostName(networkProtocol.getHostName())
                .fqdn(networkProtocol.getFqdn())
                .http(toProtocolDto(networkProtocol.getHttpProtocolEnabled(), networkProtocol.getHttpPort()))
                .https(toProtocolDto(networkProtocol.getHttpsProtocolEnabled(), networkProtocol.getHttpsPort()))
                .ipmi(toProtocolDto(networkProtocol.getIpmiProtocolEnabled(), networkProtocol.getIpmiPort()))
                .ssh(toProtocolDto(networkProtocol.getSshProtocolEnabled(), networkProtocol.getSshPort()))
                .snmp(toProtocolDto(networkProtocol.getSnmpProtocolEnabled(), networkProtocol.getSnmpPort()))
                .virtualMedia(toProtocolDto(networkProtocol.getVirtualMediaProtocolEnabled(), networkProtocol.getVirtualMediaPort()))
                .ssdp(getSsdpProtocolDto(networkProtocol))
                .telnet(toProtocolDto(networkProtocol.getTelnetProtocolEnabled(), networkProtocol.getTelnetPort()))
                .kvmip(toProtocolDto(networkProtocol.getKvmIpProtocolEnabled(), networkProtocol.getKvmIpPort()))
                .build();
    }

    private SimpleServiceDiscoveryProtocolDto getSsdpProtocolDto(NetworkProtocol networkProtocol) {
        return SimpleServiceDiscoveryProtocolDto
                .newBuilder()
                .protocolEnabled(networkProtocol.getSsdpProtocolEnabled())
                .port(networkProtocol.getSshPort())
                .notifyMulticastIntervalSeconds(networkProtocol.getSsdpNotifyMulticastIntervalSeconds())
                .notifyTtl(networkProtocol.getSsdpNotifyTtl())
                .notifyIpV6Scope(networkProtocol.getSsdpNotifyIpV6Scope())
                .build();

    }

    private ProtocolDto toProtocolDto(Boolean protocolEnabled, Integer port) {
        return ProtocolDto
                .newBuilder()
                .protocolEnabled(protocolEnabled)
                .port(port)
                .build();
    }

}
