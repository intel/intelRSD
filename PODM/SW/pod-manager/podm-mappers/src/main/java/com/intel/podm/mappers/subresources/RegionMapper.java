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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.Region;
import com.intel.podm.client.resources.redfish.MemoryRegionObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class RegionMapper extends SubresourceMapper<MemoryRegionObject, Region> {
    @Inject
    public RegionMapper(EmbeddableCleanAndCreateStrategy<Region> embeddableCleanAndCreateStrategy) {
        super(Region.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(MemoryRegionObject sourceRegion, Region targetRegion) {
        sourceRegion.getRegionId().ifAssigned(targetRegion::setRegionId);
        sourceRegion.getMemoryClassification().ifAssigned(targetRegion::setMemoryClassification);
        sourceRegion.getOffsetMib().ifAssigned(targetRegion::setOffsetMib);
        sourceRegion.getSizeMib().ifAssigned(targetRegion::setSizeMib);
    }

    @Override
    public boolean equals(MemoryRegionObject sourceRegion, Region targetRegion) {
        return Objects.equals(sourceRegion.getRegionId().orElse(targetRegion.getRegionId()), targetRegion.getRegionId())
                && Objects.equals(sourceRegion.getMemoryClassification().orElse(targetRegion.getMemoryClassification()),
                    targetRegion.getMemoryClassification())
                && Objects.equals(sourceRegion.getOffsetMib().orElse(targetRegion.getOffsetMib()),
                    targetRegion.getOffsetMib())
                && Objects.equals(sourceRegion.getSizeMib().orElse(targetRegion.getSizeMib()),
                    targetRegion.getSizeMib());
    }
}
