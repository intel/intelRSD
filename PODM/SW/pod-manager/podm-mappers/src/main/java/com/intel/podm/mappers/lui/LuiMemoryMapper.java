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

package com.intel.podm.mappers.lui;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.properties.MemoryLocation;
import com.intel.podm.client.api.resources.redfish.MemoryLocationObject;
import com.intel.podm.client.api.resources.redfish.LuiMemoryResource;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class LuiMemoryMapper extends LuiMapper<LuiMemoryResource, Memory> {
    @Inject
    private GenericDao genericDao;

    public LuiMemoryMapper() {
        super(LuiMemoryResource.class, Memory.class);
        registerProvider(MemoryLocation.class, this::provideMemoryLocation);
    }

    private MemoryLocation provideMemoryLocation(MemoryLocationObject memoryLocationObject) {
        MemoryLocation memoryLocation = target.getMemoryLocation();

        if (memoryLocation == null) {
            memoryLocation = genericDao.create(MemoryLocation.class);
        }

        return memoryLocation;
    }
}
