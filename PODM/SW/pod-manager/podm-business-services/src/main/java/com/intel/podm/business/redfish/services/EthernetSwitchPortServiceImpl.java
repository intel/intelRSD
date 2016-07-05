/*
 * Copyright (c) 2016 Intel Corporation
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
import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.EthernetSwitchPortService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORTS;
import static com.intel.podm.business.redfish.Contexts.asEthernetSwitchPortContexts;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class EthernetSwitchPortServiceImpl implements EthernetSwitchPortService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private IpAddressDtoTranslateHelper dtoTranslateHelper;

    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Override
    public CollectionDto getEthernetSwitchPortCollection(Context ethernetSwitch) throws EntityNotFoundException {
        EthernetSwitch aSwitch = (EthernetSwitch) traverser.traverse(ethernetSwitch);
        return new CollectionDto(ETHERNET_SWITCH_PORTS, getAsIdList(aSwitch.getPortCollection()));
    }

    @Override
    public EthernetSwitchPortDto getEthernetSwitchPort(Context ethernetSwitchPort) throws EntityNotFoundException {
        EthernetSwitchPort port = (EthernetSwitchPort) traverser.traverse(ethernetSwitchPort);
        EthernetInterface ethernetInterface = port.getNeighborInterface();
        return EthernetSwitchPortDto.newBuilder()
                .id(port.getId())
                .name(port.getName())
                .description(port.getDescription())
                .portId(port.getPortId())
                .status(port.getStatus())
                .portMode(port.getPortMode())
                .portType(port.getPortType())
                .linkType(port.getLinkType())
                .autosense(port.getAutosense())
                .portClass(port.getPortClass())
                .frameSize(port.getFrameSize())
                .fullDuplex(port.getFullDuplex())
                .macAddress(port.getMacAddress())
                .neighborMac(port.getNeighborMac())
                .linkSpeedMbps(port.getLinkSpeedMbps())
                .operationalState(port.getOperationalState())
                .administrativeState(port.getAdministrativeState())
                .neighborInfo(port.getNeighborInfo())
                .switchContext(toContext(port.getEthernetSwitch()))
                .ipv4Addresses(dtoTranslateHelper.translateIpV4AddressesToDto(port.getIpV4Addresses()))
                .ipv6Addresses(dtoTranslateHelper.translateIpV6AddressesToDto(port.getIpV6Addresses()))
                .portMembers(asEthernetSwitchPortContexts(port.getPortMembers()))
                .neighborInterface(ethernetInterface != null ? toContext(ethernetInterface) : null)
                .memberOfPort(port.getMemberOfPort() != null ? toContext(port.getMemberOfPort()) : null)
                .primaryVlan(port.getPrimaryVlan() != null ? toContext(port.getPrimaryVlan()) : null)
                .build();
    }
}
