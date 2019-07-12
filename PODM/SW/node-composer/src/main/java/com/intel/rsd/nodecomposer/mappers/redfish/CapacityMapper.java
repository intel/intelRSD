/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.mappers.redfish;

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.CapacityInfoResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.CapacityResource;
import com.intel.rsd.nodecomposer.mappers.subresources.SubresourceMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Capacity;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.CapacityInfo;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class CapacityMapper extends SubresourceMapper<CapacityResource, Capacity> {
    private CapacityInfoMapper capacityInfoMapper;

    @Autowired
    public CapacityMapper(CapacityInfoMapper capacityInfoMapper,
                          EmbeddableCleanAndCreateStrategy<Capacity> embeddableCleanAndCreateStrategy) {
        super(Capacity.class, embeddableCleanAndCreateStrategy);
        this.capacityInfoMapper = capacityInfoMapper;
    }

    @Override
    public void map(CapacityResource capacityResource, Capacity capacity) {
        if (capacityResource.getData() != null) {
            CapacityInfo capacityInfoData = mapCapacityInfo(capacityResource.getData());
            capacity.setData(capacityInfoData);
        }
    }

    private CapacityInfo mapCapacityInfo(CapacityInfoResource resource) {
        CapacityInfo capacityInfoData = new CapacityInfo();
        capacityInfoMapper.map(resource, capacityInfoData);
        return capacityInfoData;
    }

    @Override
    protected boolean equals(CapacityResource capacityResource, Capacity capacity) {
        return capacityInfoMapper.equals(capacityResource.getData(), capacity.getData());
    }
}
