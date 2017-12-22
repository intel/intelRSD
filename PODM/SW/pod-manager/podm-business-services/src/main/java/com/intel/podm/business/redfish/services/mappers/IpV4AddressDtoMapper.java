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

import com.intel.podm.business.dto.IpAddresses.IpV4AddressDto;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.business.services.OemSerializeHelper.oemDtoToJsonNode;

@Dependent
class IpV4AddressDtoMapper extends BrilliantMapper<IpV4Address, IpV4AddressDto> {
    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    private ExternalService externalService;

    IpV4AddressDtoMapper() {
        super(IpV4Address.class, IpV4AddressDto.class);
    }

    public void setExternalService(ExternalService externalService) {
        this.externalService = externalService;
    }

    @Override
    protected void performNotAutomatedMapping(IpV4Address source, IpV4AddressDto target) {
        super.performNotAutomatedMapping(source, target);
        target.setOem(oemDtoToJsonNode(unknownOemTranslator.translateStringOemToDto(externalService, source.getOem())));
    }
}
