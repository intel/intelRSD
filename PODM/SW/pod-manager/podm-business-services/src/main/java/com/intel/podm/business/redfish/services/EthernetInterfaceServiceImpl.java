/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.dto.redfish.EthernetInterfaceDto;
import com.intel.podm.business.dto.redfish.EthernetInterfaceDto.Builder;
import com.intel.podm.business.dto.redfish.attributes.ExtendedInfoDto;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.EthernetInterfaceDao;
import com.intel.podm.business.entities.dao.EthernetSwitchPortDao;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.base.NetworkInterfacePossessor;
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

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_INTERFACES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.common.utils.StringRepresentation.fromIterable;
import static java.util.EnumSet.of;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ClassFanOutComplexity"})
public class EthernetInterfaceServiceImpl implements ReaderService<EthernetInterfaceDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetInterfaceDao ethernetInterfaceDao;

    @Inject
    private EthernetSwitchPortDao ethernetSwitchPortDao;

    @Inject
    private IpAddressDtoTranslateHelper dtoHelper;

    @Inject
    private Logger logger;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        if (!of(MANAGER, COMPUTER_SYSTEM).contains(context.getType())) {
            throw new ContextResolvingException(context);
        }

        NetworkInterfacePossessor nicPossessor = (NetworkInterfacePossessor) traverser.traverse(context);
        return new CollectionDto(ETHERNET_INTERFACES, getAsIdSet(nicPossessor.getEthernetInterfaces()));
    }

    @Override
    public EthernetInterfaceDto getResource(Context ethernetInterfaceContext) throws ContextResolvingException {
        EthernetInterface ethernetInterface = (EthernetInterface) traverser.traverse(ethernetInterfaceContext);

        Builder ethernetInterfaceBuilder = EthernetInterfaceDto.newBuilder()
            .id(ethernetInterface.getId().toString())
            .name(ethernetInterface.getName())
            .description(ethernetInterface.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(ethernetInterface.getService(), ethernetInterface.getUnknownOems()))
            .interfaceEnabled(ethernetInterface.getInterfaceEnabled())
            .speedMbps(ethernetInterface.getSpeedMbps()).mtuSize(ethernetInterface.getMtuSize())
            .ipv6DefaultGateway(ethernetInterface.getIpV6DefaultGateway())
            .status(ethernetInterface.getStatus())
            .autoNeg(ethernetInterface.getAutoNeg()).fullDuplex(ethernetInterface.isFullDuplex())
            .vlanEnable(ethernetInterface.getVlanEnable()).vlanId(ethernetInterface.getVlanId())
            .hostname(ethernetInterface.getHostName()).fqdn(ethernetInterface.getFqdn())
            .macAddress(ethernetInterface.getMacAddress()).permanentMacAddress(ethernetInterface.getPermanentMacAddress())
            .nameServers(ethernetInterface.getNameServers())
            .ipv4Addresses(dtoHelper.translateIpV4AddressesToDto(ethernetInterface.getService(), ethernetInterface.getIpV4Addresses()))
            .ipv6Addresses(dtoHelper.translateIpV6AddressesToDto(ethernetInterface.getService(), ethernetInterface.getIpV6Addresses()))
            .ipv6StaticAddresses(dtoHelper.translateIpV6AddressesToDto(ethernetInterface.getService(), ethernetInterface.getIpV6StaticAddresses()))
            .ipV6AddressesPolicyTable(dtoHelper.translateIpV6Policies(ethernetInterface.getIpV6AddressesPolicyTable()))
            .maxIPv6StaticAddresses(ethernetInterface.getMaxIPv6StaticAddresses());

        return fillNeighborSwitchPortPart(ethernetInterfaceBuilder, ethernetInterface).build();
    }

    private Builder fillNeighborSwitchPortPart(Builder ethernetInterfaceBuilder, EthernetInterface ethernetInterface) {
        EthernetSwitchPort neighborPort = null;
        List<String> errors = new ArrayList<>();

        try {
            neighborPort = ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(ethernetInterface.getMacAddress());
        } catch (NonUniqueResultException e) {
            errors.add("Couldn't match neighbor Ethernet Switch Port. " + e.getMessage());
        }
        try {
            ethernetInterfaceDao.getEnabledAndHealthyEthernetInterfaceByMacAddress(ethernetInterface.getMacAddress());
        } catch (NonUniqueResultException e) {
            errors.add("There are different Ethernet Interfaces with the same MAC Address. " + e.getMessage());
        }

        if (errors.isEmpty()) {
            ethernetInterfaceBuilder.neighborPort(toContext(neighborPort));
        } else {
            List<ExtendedInfoDto> extendedInfoList = errors.stream()
                .map(error -> ExtendedInfoDto.newExtendedInfoDto().message(error).build())
                .collect(toList());

            ethernetInterfaceBuilder.neighborPortExtendedInfo(extendedInfoList);
            logger.e(fromIterable(errors, "\n"));
        }

        return ethernetInterfaceBuilder;
    }
}
