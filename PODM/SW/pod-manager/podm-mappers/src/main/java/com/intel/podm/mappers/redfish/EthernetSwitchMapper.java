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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.DcbxConfig;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.client.resources.redfish.DcbxConfigObject;
import com.intel.podm.client.resources.redfish.EthernetSwitchResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.ApplicationProtocolTypeMapper;
import com.intel.podm.mappers.subresources.BandwidthMappingMapper;
import com.intel.podm.mappers.subresources.PriorityClassMappingMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

import static java.util.Optional.ofNullable;

@Dependent
public class EthernetSwitchMapper extends EntityMapper<EthernetSwitchResource, EthernetSwitch> {

    @Inject
    private ApplicationProtocolTypeMapper applicationProtocolTypeMapper;

    @Inject
    private PriorityClassMappingMapper priorityClassMappingMapper;

    @Inject
    private BandwidthMappingMapper bandwidthMappingMapper;

    public EthernetSwitchMapper() {
        super(EthernetSwitchResource.class, EthernetSwitch.class);
    }

    @Override
    protected void performNotAutomatedMapping(EthernetSwitchResource source, EthernetSwitch target) {
        super.performNotAutomatedMapping(source, target);
        mapDcbxConfiguration(source, target);

    }

    private void mapDcbxConfiguration(EthernetSwitchResource source, EthernetSwitch target) {
        DcbxConfigObject sourceDcbxSharedConfig = source.getDcbxSharedConfiguration();
        if (sourceDcbxSharedConfig == null) {
            target.setDcbxSharedConfiguration(null);
            return;
        }

        DcbxConfig updatedDcbxSharedConfiguration = ofNullable(target.getDcbxSharedConfiguration())
            .orElse(new DcbxConfig());

        applicationProtocolTypeMapper.map(sourceDcbxSharedConfig.getApplicationProtocolType(),
            updatedDcbxSharedConfiguration.getApplicationProtocol(),
            updatedDcbxSharedConfiguration::addApplicationProtocol);
        priorityClassMappingMapper.map(sourceDcbxSharedConfig.getPriorityToPriorityGroupMapping(),
            updatedDcbxSharedConfiguration.getPriorityToPriorityGroupMapping(),
            updatedDcbxSharedConfiguration::addPriorityGroupToPriorityMapping);
        bandwidthMappingMapper.map(sourceDcbxSharedConfig.getBandwidthAllocation(),
            updatedDcbxSharedConfiguration.getBandwidthAllocation(),
            updatedDcbxSharedConfiguration::addBandwidthAllocation);

        target.setDcbxSharedConfiguration(updatedDcbxSharedConfiguration);
    }
}

