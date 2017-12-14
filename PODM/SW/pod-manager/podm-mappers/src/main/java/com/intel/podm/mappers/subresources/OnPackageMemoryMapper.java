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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.embeddables.OnPackageMemory;
import com.intel.podm.client.resources.redfish.ProcessorMemoryObject;
import com.intel.podm.mappers.subresources.strategies.EmbeddableCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class OnPackageMemoryMapper extends SubresourceMapper<ProcessorMemoryObject, OnPackageMemory> {
    @Inject
    public OnPackageMemoryMapper(EmbeddableCleanAndCreateStrategy<OnPackageMemory> embeddableCleanAndCreateStrategy) {
        super(OnPackageMemory.class, embeddableCleanAndCreateStrategy);
    }

    @Override
    public void map(ProcessorMemoryObject sourceProcessorMemory, OnPackageMemory onPackageMemory) {
        sourceProcessorMemory.getType().ifAssigned(onPackageMemory::setType);
        sourceProcessorMemory.getSpeedMhz().ifAssigned(onPackageMemory::setSpeedMhz);
        sourceProcessorMemory.getCapacityMb().ifAssigned(onPackageMemory::setCapacityMb);
    }

    @Override
    public boolean equals(ProcessorMemoryObject sourceProcessorMemory, OnPackageMemory onPackageMemory) {
        return Objects.equals(sourceProcessorMemory.getCapacityMb().orElse(onPackageMemory.getCapacityMb()), onPackageMemory.getCapacityMb())
                && Objects.equals(sourceProcessorMemory.getSpeedMhz().orElse(onPackageMemory.getSpeedMhz()), onPackageMemory.getSpeedMhz())
                && Objects.equals(sourceProcessorMemory.getType().orElse(onPackageMemory.getType()), onPackageMemory.getType());
    }
}
