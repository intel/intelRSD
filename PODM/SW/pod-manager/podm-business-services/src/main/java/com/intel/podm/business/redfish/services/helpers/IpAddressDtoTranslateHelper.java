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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.dto.redfish.attributes.IpV4AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressDto;
import com.intel.podm.business.dto.redfish.attributes.IpV6AddressPolicyDto;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6AddressPolicy;

import javax.ejb.Stateless;
import javax.inject.Inject;
import java.util.Collection;
import java.util.List;

import static java.util.stream.Collectors.toList;

@Stateless
public class IpAddressDtoTranslateHelper {
    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    public List<IpV4AddressDto> translateIpV4AddressesToDto(ExternalService externalService, Collection<IpV4Address> addresses) {
        return addresses.stream()
            .map(address -> buildIpV4AddressDto(externalService, address))
            .collect(toList());
    }

    private IpV4AddressDto buildIpV4AddressDto(ExternalService externalService, IpV4Address address) {
        return IpV4AddressDto.newBuilder()
            .address(address.getAddress())
            .subnetMask(address.getSubnetMask())
            .addressOrigin(address.getAddressOrigin())
            .gateway(address.getGateway())
            .oem(unknownOemTranslator.translateStringOemToDto(externalService, address.getOem()))
            .build();
    }

    public List<IpV6AddressDto> translateIpV6AddressesToDto(ExternalService externalService, Collection<IpV6Address> addresses) {
        return addresses.stream()
            .map(address -> buildIpV6AddressDto(externalService, address))
            .collect(toList());
    }

    private IpV6AddressDto buildIpV6AddressDto(ExternalService externalService, IpV6Address address) {
        return IpV6AddressDto.newBuilder()
            .address(address.getAddress())
            .addressOrigin(address.getAddressOrigin())
            .addressState(address.getAddressState())
            .prefixLength(address.getPrefixLength())
            .oem(unknownOemTranslator.translateStringOemToDto(externalService, address.getOem()))
            .build();
    }

    public List<IpV6AddressPolicyDto> translateIpV6Policies(List<IpV6AddressPolicy> policies) {
        return policies.stream()
            .map(this::buildIpV6AddressPolicyDto)
            .collect(toList());
    }

    private IpV6AddressPolicyDto buildIpV6AddressPolicyDto(IpV6AddressPolicy policy) {
        return IpV6AddressPolicyDto.newBuilder()
            .prefix(policy.getPrefix())
            .label(policy.getLabel())
            .precedence(policy.getPrecedence())
            .build();
    }
}
