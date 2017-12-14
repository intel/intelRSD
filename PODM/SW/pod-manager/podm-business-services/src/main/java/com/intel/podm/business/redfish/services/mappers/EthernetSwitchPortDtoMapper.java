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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.EthernetSwitchPortDto;
import com.intel.podm.business.dto.IpAddresses;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6Address;
import com.intel.podm.business.redfish.services.helpers.NeighborFinder;
import com.intel.podm.common.types.redfish.RedfishErrorResponse.Error.ExtendedInfo;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;

import static com.intel.podm.business.errors.ExtendedInfoDto.ExtendedInfoBuilder.newExtendedInfoBuilder;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchAclContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetSwitchPortContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toList;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class EthernetSwitchPortDtoMapper extends DtoMapper<EthernetSwitchPort, EthernetSwitchPortDto> {
    @Inject
    private NeighborFinder neighborFinder;

    @Inject
    private IpV4AddressDtoMapper ipV4AddressDtoMapper;

    @Inject
    private IpV6AddressDtoMapper ipV6AddressDtoMapper;

    EthernetSwitchPortDtoMapper() {
        super(EthernetSwitchPort.class, EthernetSwitchPortDto.class);
        this.ignoredProperties("ipV4Addresses", "ipV6Addresses", "vlans", "staticMacs", "links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchPort source, EthernetSwitchPortDto target) {
        super.performNotAutomatedMapping(source, target);

        EthernetSwitchPortDto.Links links = target.getLinks();
        links.setSwitchContext(toContext(source.getEthernetSwitch()));
        links.getPortMembers().addAll(asEthernetSwitchPortContexts(source.getPortMembers()));
        links.setMemberOfPort(toContext(source.getMemberOfPort()));
        links.setPrimaryVlan(toContext(source.getPrimaryVlan()));
        links.getActiveAcls().addAll(asEthernetSwitchAclContexts(source.getActiveAcls()));

        ExternalService sourceService = source.getService();
        if (sourceService == null || !source.isComplementary()) {
            mapNeighborEthernetInterface(source, target);
        }

        ipV4AddressDtoMapper.setExternalService(sourceService);
        mapIpV4Addresses(source, target);

        ipV6AddressDtoMapper.setExternalService(sourceService);
        mapIpV6Addresses(source, target);
    }

    private void mapNeighborEthernetInterface(EthernetSwitchPort source, EthernetSwitchPortDto target) {
        try {
            EthernetInterface neighborEthernetInterface = neighborFinder.tryFindNeighborEthernetInterface(source);
            target.setNeighborInterface(toContext(neighborEthernetInterface));
        } catch (NeighborFinder.NeighborFinderException e) {
            List<ExtendedInfo> extendedInfoList = e.getNeighborFinderErrors().stream()
                .map(error -> newExtendedInfoBuilder(error).build())
                .collect(toList());
            target.setNeighborInterfaceExtendedInfo(extendedInfoList);
        }
    }

    private void mapIpV4Addresses(EthernetSwitchPort source, EthernetSwitchPortDto target) {
        for (IpV4Address ipV4Address : source.getIpV4Addresses()) {
            IpAddresses.IpV4AddressDto ipV4AddressDto = new IpAddresses.IpV4AddressDto();
            ipV4AddressDtoMapper.map(ipV4Address, ipV4AddressDto);
            target.getIpV4Addresses().add(ipV4AddressDto);
        }
    }

    private void mapIpV6Addresses(EthernetSwitchPort source, EthernetSwitchPortDto target) {
        for (IpV6Address ipV6Address : source.getIpV6Addresses()) {
            IpAddresses.IpV6AddressDto ipV6AddressDto = new IpAddresses.IpV6AddressDto();
            ipV6AddressDtoMapper.map(ipV6Address, ipV6AddressDto);
            target.getIpV6Addresses().add(ipV6AddressDto);
        }
    }
}
