/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.properties.Region;
import com.intel.podm.client.api.resources.redfish.MemoryResource;
import com.intel.podm.client.api.resources.redfish.MemoryRegionObject;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.function.Supplier;

import static java.util.Objects.isNull;

@Dependent
public class MemoryMapper extends DomainObjectMapper<MemoryResource, Memory> {

    @Inject
    private GenericDao genericDao;

    public MemoryMapper() {
        super(MemoryResource.class, Memory.class);
    }

    @Override
    protected void performNotAutomatedMapping(MemoryResource source, Memory target) {
        clearRegions(target.getRegions());
        addRegions(source.getRegions(), target::addRegion);
    }

    private void clearRegions(Collection<Region> regions) {
        regions.forEach(genericDao::remove);
    }

    private void addRegions(Iterable<MemoryRegionObject> regions, Supplier<Region> regionSupplier) {
        if (isNull(regions)) {
            return;
        }

        for (MemoryRegionObject region : regions) {
            Region reg = regionSupplier.get();
            reg.setRegionId(region.getRegionId());
            reg.setMemoryClassification(region.getMemoryClassification());
            reg.setOffsetMib(region.getOffsetMib());
            reg.setSizeMib(region.getSizeMib());
        }
    }
}
