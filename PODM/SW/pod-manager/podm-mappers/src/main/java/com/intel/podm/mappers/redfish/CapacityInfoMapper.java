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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.embeddables.CapacityInfo;
import com.intel.podm.client.resources.redfish.CapacityInfoResource;
import com.intel.podm.mappers.subresources.SubresourceMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class CapacityInfoMapper extends SubresourceMapper<CapacityInfoResource, CapacityInfo> {
    @Inject
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

        if (capacityInfoResource.getGuaranteedBytes() != null) {
            capacityInfo.setGuaranteedBytes(capacityInfoResource.getGuaranteedBytes());
        }

        if (capacityInfoResource.getProvisionedBytes() != null) {
            capacityInfo.setProvisionedBytes(capacityInfoResource.getProvisionedBytes());
        }
    }

    @Override
    protected boolean equals(CapacityInfoResource capacityInfoResource, CapacityInfo capacityInfo) {
        return Objects.equals(capacityInfoResource.getAllocatedBytes(), capacityInfo.getAllocatedBytes())
            && Objects.equals(capacityInfoResource.getConsumedBytes(), capacityInfo.getConsumedBytes())
            && Objects.equals(capacityInfoResource.getGuaranteedBytes(), capacityInfo.getGuaranteedBytes())
            && Objects.equals(capacityInfoResource.getProvisionedBytes(), capacityInfo.getProvisionedBytes());
    }
}
