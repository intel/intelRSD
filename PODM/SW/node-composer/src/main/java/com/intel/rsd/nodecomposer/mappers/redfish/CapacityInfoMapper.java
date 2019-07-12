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
import com.intel.rsd.nodecomposer.mappers.subresources.SubresourceMapper;
import com.intel.rsd.nodecomposer.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.CapacityInfo;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Objects;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class CapacityInfoMapper extends SubresourceMapper<CapacityInfoResource, CapacityInfo> {
    @Autowired
    public CapacityInfoMapper(EmbeddableCleanAndCreateStrategy<CapacityInfo> embeddableCleanAndCreateStrategy) {
        super(CapacityInfo.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(CapacityInfoResource capacityInfoResource, CapacityInfo capacityInfo) {
        if (capacityInfoResource.getAllocatedBytes() != null) {
            capacityInfo.setAllocatedBytes(capacityInfoResource.getAllocatedBytes());
        }

        if (capacityInfoResource.getConsumedBytes() != null) {
            capacityInfo.setConsumedBytes(capacityInfoResource.getConsumedBytes());
        }
    }

    @Override
    protected boolean equals(CapacityInfoResource capacityInfoResource, CapacityInfo capacityInfo) {
        return Objects.equals(capacityInfoResource.getAllocatedBytes(), capacityInfo.getAllocatedBytes())
            && Objects.equals(capacityInfoResource.getConsumedBytes(), capacityInfo.getConsumedBytes());
    }
}
