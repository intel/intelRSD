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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.EndpointDto.IpTransportDetailsDto;
import com.intel.podm.business.dto.IpAddresses;
import com.intel.podm.business.entities.redfish.IpTransportDetails;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
class IpTransportDetailsDtoMapper extends BrilliantMapper<IpTransportDetails, IpTransportDetailsDto> {

    @Inject
    private IpV4AddressDtoMapper ipV4AddressDtoMapper;

    @Inject
    private IpV6AddressDtoMapper ipV6AddressDtoMapper;

    IpTransportDetailsDtoMapper() {
        super(IpTransportDetails.class, IpTransportDetailsDto.class);
    }

    @Override
    protected void performNotAutomatedMapping(IpTransportDetails source, IpTransportDetailsDto target) {
        super.performNotAutomatedMapping(source, target);

        mapIpv4Address(source, target);
        mapIpv6Address(source, target);

    }

    private void mapIpv6Address(IpTransportDetails source, IpTransportDetailsDto target) {
        if (source.getIpv6Address() != null) {
            IpAddresses.IpV6AddressDto ipV6AddressDto = new IpAddresses.IpV6AddressDto();
            ipV6AddressDtoMapper.map(source.getIpv6Address(), ipV6AddressDto);
            target.setIpv6Address(ipV6AddressDto);
        }
    }

    private void mapIpv4Address(IpTransportDetails source, IpTransportDetailsDto target) {
        if (source.getIpv4Address() != null) {
            IpAddresses.IpV4AddressDto ipV4AddressDto = new IpAddresses.IpV4AddressDto();
            ipV4AddressDtoMapper.map(source.getIpv4Address(), ipV4AddressDto);
            target.setIpv4Address(ipV4AddressDto);
        }
    }
}
