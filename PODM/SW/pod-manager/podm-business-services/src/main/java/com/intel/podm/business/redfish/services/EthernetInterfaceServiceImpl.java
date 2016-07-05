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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EthernetInterfaceDto;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.properties.NameServer;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.EthernetInterfaceService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_INTERFACES;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class EthernetInterfaceServiceImpl implements EthernetInterfaceService {

    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private IpAddressDtoTranslateHelper dtoHelper;

    @Inject
    private EthernetSwitchPortDao switchPortDao;

    @Override
    public CollectionDto getEthernetInterfacesCollectionForComputerSystem(Context systemContext) throws EntityNotFoundException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(systemContext);
        return new CollectionDto(ETHERNET_INTERFACES, Contexts.getAsIdList(system.getEthernetInterfaces()));
    }

    @Override
    public CollectionDto getEthernetInterfacesCollectionForManager(Context managerContext) throws EntityNotFoundException {
        Manager manager = (Manager) traverser.traverse(managerContext);
        return new CollectionDto(ETHERNET_INTERFACES, Contexts.getAsIdList(manager.getEthernetInterfaces()));
    }

    @Override
    public EthernetInterfaceDto getEthernetInterface(Context ethernetInterfaceContext) throws EntityNotFoundException {
        EthernetInterface ethernetInterface = (EthernetInterface) traverser.traverse(ethernetInterfaceContext);
        EthernetSwitchPort neighborPort = ethernetInterface.getNeighborSwitchPort();
        return EthernetInterfaceDto.newBuilder()
                .id(ethernetInterface.getId()).name(ethernetInterface.getName())
                .interfaceEnabled(ethernetInterface.getInterfaceEnabled())
                .speedMbps(ethernetInterface.getSpeedMbps()).mtuSize(ethernetInterface.getMtuSize())
                .ipv6DefaultGateway(ethernetInterface.getIpV6DefaultGateway())
                .description(ethernetInterface.getDescription()).status(ethernetInterface.getStatus())
                .autoNeg(ethernetInterface.getAutoNeg()).fullDuplex(ethernetInterface.isFullDuplex())
                .vlanEnable(ethernetInterface.getVlanEnable()).vlanId(ethernetInterface.getVlanId())
                .hostname(ethernetInterface.getHostName()).fqdn(ethernetInterface.getFqdn())
                .macAddress(ethernetInterface.getMacAddress()).permanentMacAddress(ethernetInterface.getPermanentMacAddress())
                .nameServers(getNameServers(ethernetInterface))
                .ipv4Addresses(dtoHelper.translateIpV4AddressesToDto(ethernetInterface.getIpV4Addresses()))
                .ipv6Addresses(dtoHelper.translateIpV6AddressesToDto(ethernetInterface.getIpV6Addresses()))
                .ipv6StaticAddresses(dtoHelper.translateIpV6AddressesToDto(ethernetInterface.getIpV6StaticAddresses()))
                .ipV6AddressesPolicyTable(dtoHelper.translateIpV6Policies(ethernetInterface.getIpV6AddressesPolicyTable()))
                .maxIPv6StaticAddresses(ethernetInterface.getMaxIPv6StaticAddresses())
                .neighborPort(neighborPort != null ? Contexts.toContext(neighborPort) : null)
                .build();
    }

    private List<String> getNameServers(EthernetInterface ethernetInterface) {
        return ethernetInterface.getNameServers().stream()
                .map(NameServer::getNameServer)
                .collect(toList());
    }
}
