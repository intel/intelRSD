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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.EthernetInterfaceDto;
import com.intel.podm.business.dto.redfish.attributes.IpV4AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressPolicyDto;
import com.intel.podm.redfish.json.templates.EthernetInterfaceJson;
import com.intel.podm.redfish.json.templates.EthernetInterfaceJson.Links.Oem.RackScaleOem;
import com.intel.podm.rest.odataid.ODataIds;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.List;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static java.net.URI.create;
import static java.util.Collections.emptyList;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

public class EthernetInterfaceJsonSerializer extends DtoJsonSerializer<EthernetInterfaceDto> {
    public EthernetInterfaceJsonSerializer() {
        super(EthernetInterfaceDto.class);
    }

    @Override
    protected EthernetInterfaceJson translate(EthernetInterfaceDto dto) {
        EthernetInterfaceJson ifaceJson = new EthernetInterfaceJson();
        ifaceJson.oDataContext = getContextFromId(oDataId(context.getRequestPath()));
        ifaceJson.id = dto.getId();
        ifaceJson.name = dto.getName();
        ifaceJson.description = dto.getDescription();
        ifaceJson.fqdn = dto.getFqdn();
        ifaceJson.hostname = dto.getHostname();
        ifaceJson.status = dto.getStatus();
        ifaceJson.interfaceEnabled = dto.getInterfaceEnabled();
        ifaceJson.oDataId = oDataId(context.getRequestPath());
        ifaceJson.speedMbps = dto.getSpeedMbps();
        ifaceJson.autoNeg = dto.getAutoNeg();
        ifaceJson.fullDuplex = dto.getFullDuplex();
        ifaceJson.mtuSize = dto.getMtuSize();
        ifaceJson.permanentMacAddress = dto.getPermanentMacAddress();
        ifaceJson.macAddress = dto.getMacAddress();
        handleVlans(dto, ifaceJson);
        ifaceJson.nameServers = dto.getNameServers();
        if (dto.getNeighborPort() != null) {
            ifaceJson.links.oem.rackScaleOem = new RackScaleOem(ODataIds.oDataIdFromContext(dto.getNeighborPort()));
        }
        handleIpAddresses(dto, ifaceJson);
        return ifaceJson;
    }

    private void handleVlans(EthernetInterfaceDto dto, EthernetInterfaceJson ifaceJson) {
        ifaceJson.vlans = oDataId(create(ifaceJson.oDataId + "/VLANs"));
    }

    private void handleIpAddresses(EthernetInterfaceDto dto, EthernetInterfaceJson ifaceJson) {
        ifaceJson.ipv4Addresses = mapToIpv4Addresses(dto.getIpv4Addresses());
        ifaceJson.ipv6Addresses = mapToIpv6Addresses(dto.getIpv6Addresses());
        ifaceJson.ipv6StaticAddresses = mapToIpv6Addresses(dto.getIpv6StaticAddresses());
        ifaceJson.ipV6AddressPolicyTable.addAll(mapToIpV6AddressesPolicyTable(dto.getIpV6AddressesPolicyTable()));
        ifaceJson.maxIPv6StaticAddresses = dto.getMaxIPv6StaticAddresses();
        ifaceJson.ipv6DefaultGateway = dto.getIpv6DefaultGateway();
    }

    private List<EthernetInterfaceJson.IpV6AddressPolicyJson> mapToIpV6AddressesPolicyTable(List<IpV6AddressPolicyDto> policies) {
        return isNull(policies) ? emptyList() : policies.stream()
                .map(policy -> {
                    EthernetInterfaceJson.IpV6AddressPolicyJson policiesJson = new EthernetInterfaceJson.IpV6AddressPolicyJson();
                    policiesJson.label = policy.getLabel();
                    policiesJson.precedence = policy.getPrecedence();
                    policiesJson.prefix = policy.getPrefix();
                    return policiesJson;
                }).collect(toList());
    }

    static List<EthernetInterfaceJson.Ipv6AddressJson> mapToIpv6Addresses(List<IpV6AddressDto> ipv6Addresses) {
        return isNull(ipv6Addresses) ? emptyList() : ipv6Addresses.stream()
                .map(ipv6AddressDto -> {
                    EthernetInterfaceJson.Ipv6AddressJson ipv6AddressJson = new EthernetInterfaceJson.Ipv6AddressJson();
                    ipv6AddressJson.address = ipv6AddressDto.getAddress();
                    ipv6AddressJson.addressState = ipv6AddressDto.getAddressState();
                    ipv6AddressJson.prefixLength = ipv6AddressDto.getPrefixLength();
                    ipv6AddressJson.addressOrigin = ipv6AddressDto.getAddressOrigin();
                    return ipv6AddressJson;
                }).collect(toList());
    }


    static List<EthernetInterfaceJson.Ipv4AddressJson> mapToIpv4Addresses(List<IpV4AddressDto> ipv4Addresses) {
        return isNull(ipv4Addresses) ? emptyList() : ipv4Addresses.stream()
                .map(ipv4AddressDto -> {
                    EthernetInterfaceJson.Ipv4AddressJson ipv4AddressJson = new EthernetInterfaceJson.Ipv4AddressJson();
                    ipv4AddressJson.address = ipv4AddressDto.getAddress();
                    ipv4AddressJson.gateway = ipv4AddressDto.getGateway();
                    ipv4AddressJson.subnetMask = ipv4AddressDto.getSubnetMask();
                    ipv4AddressJson.addressOrigin = ipv4AddressDto.getAddressOrigin();
                    return ipv4AddressJson;
                }).collect(toList());
    }
}
