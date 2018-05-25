/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.dto.DcbxConfigDto.ApplicationProtocolDto;
import com.intel.podm.business.dto.DcbxConfigDto.PriorityClassMappingDto;
import com.intel.podm.business.dto.DcbxConfigDto.BandwidthMappingDto;
import com.intel.podm.business.dto.DcbxConfigDto;
import com.intel.podm.business.dto.EthernetSwitchDto;
import com.intel.podm.business.entities.redfish.DcbxConfig;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.embeddables.ApplicationProtocolType;
import com.intel.podm.business.entities.redfish.embeddables.BandwidthMapping;
import com.intel.podm.business.entities.redfish.embeddables.PriorityClassMapping;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class EthernetSwitchDtoMapper extends DtoMapper<EthernetSwitch, EthernetSwitchDto> {

    @Inject
    private ApplicationProtocolDtoMapper applicationProtocolDtoMapper;

    @Inject
    private PriorityClassMappingDtoMapper priorityClassMappingDtoMapper;

    @Inject
    private BandwidthMappingDtoMapper bandwidthMappingDtoMapper;

    EthernetSwitchDtoMapper() {
        super(EthernetSwitch.class, EthernetSwitchDto.class);
        this.ignoredProperties("applicationProtocol", "priorityToPriorityGroupMapping", "bandwidthMapping");
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitch source, EthernetSwitchDto target) {
        super.performNotAutomatedMapping(source, target);
        target.setLinks(target.createLinks(toContext(source.getChassis()), asManagerContexts(source.getManagers())));
        mapDcbxSharedConfiguration(source, target);
    }

    private void mapDcbxSharedConfiguration(EthernetSwitch source, EthernetSwitchDto target) {
        if (source.getDcbxSharedConfiguration() == null) {
            target.setDcbxSharedConfigurationDto(null);
            return;
        }
        mapApplicationProtocol(source.getDcbxSharedConfiguration(), target.getDcbxSharedConfigurationDto());
        mapPriorityClassMapping(source.getDcbxSharedConfiguration(), target.getDcbxSharedConfigurationDto());
        mapBandwidthMapping(source.getDcbxSharedConfiguration(), target.getDcbxSharedConfigurationDto());
    }

    private void mapApplicationProtocol(DcbxConfig source, DcbxConfigDto target) {
        for (ApplicationProtocolType applicationProtocolType : source.getApplicationProtocol()) {
            ApplicationProtocolDto applicationProtocolDto = new ApplicationProtocolDto();
            applicationProtocolDtoMapper.map(applicationProtocolType, applicationProtocolDto);
            target.getApplicationProtocol().add(applicationProtocolDto);
        }
    }

    private void mapPriorityClassMapping(DcbxConfig source, DcbxConfigDto target) {
        for (PriorityClassMapping priorityClassMapping : source.getPriorityToPriorityGroupMapping()) {
            PriorityClassMappingDto priorityClassMappingDto = new PriorityClassMappingDto();
            priorityClassMappingDtoMapper.map(priorityClassMapping, priorityClassMappingDto);
            target.getPriorityToPriorityGroupMapping().add(priorityClassMappingDto);
        }
    }

    private void mapBandwidthMapping(DcbxConfig source, DcbxConfigDto target) {
        for (BandwidthMapping bandwidthMapping : source.getBandwidthAllocation()) {
            BandwidthMappingDto bandwidthMappingDto = new BandwidthMappingDto();
            bandwidthMappingDtoMapper.map(bandwidthMapping, bandwidthMappingDto);
            target.getBandwidthAllocation().add(bandwidthMappingDto);
        }
    }
}
