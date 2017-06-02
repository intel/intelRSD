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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.EthernetInterfaceDto;
import com.intel.podm.business.dto.redfish.attributes.IpV4AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressPolicyDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.redfish.json.templates.EthernetInterfaceJson;
import com.intel.podm.redfish.json.templates.EthernetInterfaceJson.IpV6AddressPolicyJson;
import com.intel.podm.redfish.json.templates.EthernetInterfaceJson.Ipv4AddressJson;
import com.intel.podm.redfish.json.templates.EthernetInterfaceJson.Ipv6AddressJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;
import com.intel.podm.rest.representation.json.templates.RedfishErrorResponseJson.ExtendedInfoJson;

import java.util.List;

import static com.intel.podm.common.utils.Collections.nullOrEmpty;
import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static com.intel.podm.rest.representation.json.serializers.OemSerializeHelper.oemDtoToJsonNode;
import static java.net.URI.create;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public class EthernetInterfaceJsonSerializer extends BaseDtoJsonSerializer<EthernetInterfaceDto> {
    public EthernetInterfaceJsonSerializer() {
        super(EthernetInterfaceDto.class);
    }

    @Override
    protected EthernetInterfaceJson translate(EthernetInterfaceDto dto) {
        EthernetInterfaceJson ifaceJson = new EthernetInterfaceJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        ifaceJson.oDataId = oDataId;
        ifaceJson.oDataContext = getContextFromId(oDataId);
        ifaceJson.id = dto.getId();
        ifaceJson.name = dto.getName();
        ifaceJson.description = dto.getDescription();
        ifaceJson.fqdn = dto.getFqdn();
        ifaceJson.hostname = dto.getHostname();
        ifaceJson.status = dto.getStatus();
        ifaceJson.interfaceEnabled = dto.getInterfaceEnabled();
        ifaceJson.speedMbps = dto.getSpeedMbps();
        ifaceJson.autoNeg = dto.getAutoNeg();
        ifaceJson.fullDuplex = dto.getFullDuplex();
        ifaceJson.mtuSize = dto.getMtuSize();
        ifaceJson.permanentMacAddress = dto.getPermanentMacAddress();
        ifaceJson.macAddress = dto.getMacAddress();
        ifaceJson.vlans = oDataIdFromUri(create(ifaceJson.oDataId + "/VLANs"));
        ifaceJson.nameServers = dto.getNameServers();
        handleNeighborPort(dto, ifaceJson);
        handleIpAddresses(dto, ifaceJson);
        return ifaceJson;
    }

    private void handleNeighborPort(EthernetInterfaceDto dto, EthernetInterfaceJson ifaceJson) {
        Context neighborPortContext = dto.getLinks().getNeighborPort();
        List<ExtendedInfoJson> neighborPortExtendedInfoJson =
            nullOrEmpty(dto.getLinks().getNeighborPortExtendedInfo()) ? null : dto.getLinks().getNeighborPortExtendedInfo().stream()
                .map(extendedInfo -> new ExtendedInfoJson(extendedInfo.getMessageId(), extendedInfo.getMessage()))
                .collect(toList());

        if (neighborPortContext != null || neighborPortExtendedInfoJson != null) {
            EthernetInterfaceJson.Links.Oem.RackScaleOem rackScaleOem = ifaceJson.links.oem.createNewRackScaleOem();
            rackScaleOem.neighborPort = asOdataId(neighborPortContext);
            rackScaleOem.neighborPortExtendedInfo = neighborPortExtendedInfoJson;
            ifaceJson.links.oem.rackScaleOem = rackScaleOem;
        }
    }

    private void handleIpAddresses(EthernetInterfaceDto dto, EthernetInterfaceJson ifaceJson) {
        ifaceJson.ipv4Addresses = mapToIpv4Addresses(dto.getIpv4Addresses());
        ifaceJson.ipv6Addresses = mapToIpv6Addresses(dto.getIpv6Addresses());
        ifaceJson.ipv6StaticAddresses = mapToIpv6Addresses(dto.getIpv6StaticAddresses());
        ifaceJson.ipV6AddressPolicyTable.addAll(mapToIpV6AddressesPolicyTable(dto.getIpV6AddressesPolicyTable()));
        ifaceJson.maxIPv6StaticAddresses = dto.getMaxIPv6StaticAddresses();
        ifaceJson.ipv6DefaultGateway = dto.getIpv6DefaultGateway();
    }

    private List<IpV6AddressPolicyJson> mapToIpV6AddressesPolicyTable(List<IpV6AddressPolicyDto> policies) {
        return policies == null ? emptyList() : policies.stream()
            .map(policy -> {
                IpV6AddressPolicyJson policiesJson = new IpV6AddressPolicyJson();
                policiesJson.label = policy.getLabel();
                policiesJson.precedence = policy.getPrecedence();
                policiesJson.prefix = policy.getPrefix();
                return policiesJson;
            }).collect(toList());
    }

    static List<Ipv6AddressJson> mapToIpv6Addresses(List<IpV6AddressDto> ipv6Addresses) {
        return ipv6Addresses == null ? emptyList() : ipv6Addresses.stream()
            .map(ipv6AddressDto -> {
                Ipv6AddressJson ipv6AddressJson = new Ipv6AddressJson();
                ipv6AddressJson.address = ipv6AddressDto.getAddress();
                ipv6AddressJson.addressState = ipv6AddressDto.getAddressState();
                ipv6AddressJson.prefixLength = ipv6AddressDto.getPrefixLength();
                ipv6AddressJson.addressOrigin = ipv6AddressDto.getAddressOrigin();
                ipv6AddressJson.oem = oemDtoToJsonNode(ipv6AddressDto.getOem());
                return ipv6AddressJson;
            }).collect(toList());
    }

    static List<Ipv4AddressJson> mapToIpv4Addresses(List<IpV4AddressDto> ipv4Addresses) {
        return ipv4Addresses == null ? emptyList() : ipv4Addresses.stream()
            .map(ipv4AddressDto -> {
                Ipv4AddressJson ipv4AddressJson = new Ipv4AddressJson();
                ipv4AddressJson.address = ipv4AddressDto.getAddress();
                ipv4AddressJson.gateway = ipv4AddressDto.getGateway();
                ipv4AddressJson.subnetMask = ipv4AddressDto.getSubnetMask();
                ipv4AddressJson.addressOrigin = ipv4AddressDto.getAddressOrigin();
                ipv4AddressJson.oem = oemDtoToJsonNode(ipv4AddressDto.getOem());
                return ipv4AddressJson;
            }).collect(toList());
    }
}
