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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto;
import com.intel.podm.business.dto.redfish.EthernetSwitchPortDto.Builder;
import com.intel.podm.business.dto.redfish.attributes.ExtendedInfoDto;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.IpAddressDtoTranslateHelper;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.logger.Logger;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORTS;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchPortContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.utils.StringRepresentation.fromIterable;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ClassFanOutComplexity"})
public class EthernetSwitchPortServiceImpl implements ReaderService<EthernetSwitchPortDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private IpAddressDtoTranslateHelper dtoTranslateHelper;

    @Inject
    private Logger logger;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context ethernetSwitch) throws ContextResolvingException {
        EthernetSwitch aSwitch = (EthernetSwitch) traverser.traverse(ethernetSwitch);
        return new CollectionDto(ETHERNET_SWITCH_PORTS, getAsIdSet(aSwitch.getPorts()));
    }

    @Override
    public EthernetSwitchPortDto getResource(Context ethernetSwitchPort) throws ContextResolvingException {
        EthernetSwitchPort port = (EthernetSwitchPort) traverser.traverse(ethernetSwitchPort);

        Builder ethernetSwitchPortBuilder = EthernetSwitchPortDto.newBuilder()
            .id(port.getId().toString())
            .name(port.getName())
            .description(port.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(port.getService(), port.getUnknownOems()))
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
            .ipv4Addresses(dtoTranslateHelper.translateIpV4AddressesToDto(port.getService(), port.getIpV4Addresses()))
            .ipv6Addresses(dtoTranslateHelper.translateIpV6AddressesToDto(port.getService(), port.getIpV6Addresses()))
            .portMembers(asEthernetSwitchPortContexts(port.getPortMembers()))
            .memberOfPort(toContext(port.getMemberOfPort()))
            .primaryVlan(toContext(port.getPrimaryVlan()));

        return fillNeighborInterfacePart(ethernetSwitchPortBuilder, port).build();
    }

    private Builder fillNeighborInterfacePart(Builder ethernetSwitchPortBuilder, EthernetSwitchPort port) {
        EthernetInterface neighborInterface = null;
        List<String> errors = new ArrayList<>();

        try {
            neighborInterface = ethernetInterfaceDao.getEnabledAndHealthyEthernetInterfaceByMacAddress(port.getNeighborMac());
        } catch (NonUniqueResultException e) {
            errors.add("Couldn't match neighbor Ethernet Interface. " + e.getMessage());
        }
        try {
            ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(port.getNeighborMac());
        } catch (NonUniqueResultException e) {
            errors.add("There are different Ethernet Switch Ports with the same Neighbor MAC Address. " + e.getMessage());
        }

        if (errors.isEmpty()) {
            ethernetSwitchPortBuilder.neighborInterface(toContext(neighborInterface));
        } else {
            List<ExtendedInfoDto> extendedInfoList = errors.stream()
                .map(error -> ExtendedInfoDto.newExtendedInfoDto().message(error).build())
                .collect(toList());

            ethernetSwitchPortBuilder.neighborInterfaceExtendedInfo(extendedInfoList);
            logger.e(fromIterable(errors, "\n"));
        }

        return ethernetSwitchPortBuilder;
    }
}
