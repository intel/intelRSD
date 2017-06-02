/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.embeddables.MemoryLocation;
import com.intel.podm.client.api.resources.redfish.MemoryResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.RegionMapper;
import com.intel.podm.mappers.subresources.SimpleTypeMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class MemoryMapper extends EntityMapper<MemoryResource, Memory> {
    @Inject
    RegionMapper regionMapper;
    @Inject
    SimpleTypeMapper simpleTypeMapper;

    public MemoryMapper() {
        super(MemoryResource.class, Memory.class);
        registerProvider(MemoryLocation.class, target -> provideMemoryLocation());
    }

    private MemoryLocation provideMemoryLocation() {
        MemoryLocation memoryLocation = target.getMemoryLocation();
        return memoryLocation == null ? new MemoryLocation() : memoryLocation;
    }

    @Override
    protected void performNotAutomatedMapping(MemoryResource sourceMemory, Memory targetMemory) {
        super.performNotAutomatedMapping(source, target);
        source.getRegions().ifAssigned(regions ->
            regionMapper.map(regions, targetMemory.getRegions(), targetMemory::addRegion));
        source.getMemoryMedia().ifAssigned(memoryMedia ->
            simpleTypeMapper.map(memoryMedia, targetMemory.getMemoryMedia(), targetMemory::addMemoryMedia));
        source.getAllowedSpeedsMhz().ifAssigned(allowedSpeedsMhz ->
            simpleTypeMapper.map(allowedSpeedsMhz, targetMemory.getAllowedSpeedsMhz(), targetMemory::addAllowedSpeedMhz));
        source.getFunctionClasses().ifAssigned(functionClasses ->
            simpleTypeMapper.map(functionClasses, targetMemory.getFunctionClasses(), targetMemory::addFunctionClass));
        source.getOperatingMemoryModes().ifAssigned(operatingMemoryModes ->
            simpleTypeMapper.map(operatingMemoryModes, targetMemory.getOperatingMemoryModes(), targetMemory::addOperatingMemoryMode));
    }
}
