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

import com.intel.podm.business.entities.redfish.embeddables.Capacity;
import com.intel.podm.business.entities.redfish.embeddables.CapacityInfo;
import com.intel.podm.client.resources.redfish.CapacityInfoResource;
import com.intel.podm.client.resources.redfish.CapacityResource;
import com.intel.podm.mappers.subresources.SubresourceMapper;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class CapacityMapper extends SubresourceMapper<CapacityResource, Capacity> {

    private CapacityInfoMapper capacityInfoMapper;

    @Inject
    public CapacityMapper(CapacityInfoMapper capacityInfoMapper,
                          EmbeddableCleanAndCreateStrategy<Capacity> embeddableCleanAndCreateStrategy) {
        super(Capacity.class, embeddableCleanAndCreateStrategy);
        this.capacityInfoMapper = capacityInfoMapper;
    }

    @Override
    public void map(CapacityResource capacityResource, Capacity capacity) {

        if (capacityResource.getThinProvisioned() != null) {
            capacity.setThinProvisioned(capacityResource.getThinProvisioned());
        }

        if (capacityResource.getData() != null) {
            CapacityInfo capacityInfoData = mapCapacityInfo(capacityResource.getData());
            capacity.setData(capacityInfoData);
        }

        if (capacityResource.getMetadata() != null) {
            CapacityInfo capacityInfoMetadata = mapCapacityInfo(capacityResource.getMetadata());
            capacity.setMetadata(capacityInfoMetadata);
        }

        if (capacityResource.getSnapshot() != null) {
            CapacityInfo capacityInfoSnapshot = mapCapacityInfo(capacityResource.getSnapshot());
            capacity.setSnapshot(capacityInfoSnapshot);
        }
    }

    private CapacityInfo mapCapacityInfo(CapacityInfoResource resource) {
        CapacityInfo capacityInfoData = new CapacityInfo();
        capacityInfoMapper.map(resource, capacityInfoData);
        return capacityInfoData;
    }

    @Override
    protected boolean equals(CapacityResource capacityResource, Capacity capacity) {
        return Objects.equals(capacityResource.getThinProvisioned(), capacity.getThinProvisioned())
            && capacityInfoMapper.equals(capacityResource.getData(), capacity.getData())
            && capacityInfoMapper.equals(capacityResource.getMetadata(), capacity.getMetadata())
            && capacityInfoMapper.equals(capacityResource.getSnapshot(), capacity.getSnapshot());
    }
}
