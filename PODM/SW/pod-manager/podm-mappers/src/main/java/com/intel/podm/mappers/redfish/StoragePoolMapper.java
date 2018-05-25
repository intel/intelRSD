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

import com.intel.podm.business.entities.redfish.StoragePool;
import com.intel.podm.business.entities.redfish.embeddables.Capacity;
import com.intel.podm.client.resources.redfish.StoragePoolResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.IdentifierMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class StoragePoolMapper extends EntityMapper<StoragePoolResource, StoragePool> {
    @Inject
    private CapacityMapper capacityMapper;

    @Inject
    private IdentifierMapper identifierMapper;

    protected StoragePoolMapper() {
        super(StoragePoolResource.class, StoragePool.class);
    }

    @Override
    protected void performNotAutomatedMapping(StoragePoolResource source, StoragePool target) {
        super.performNotAutomatedMapping(source, target);

        mapCapacity(source);
        identifierMapper.map(source.getIdentifier(), target.getIdentifier());
    }

    private void mapCapacity(StoragePoolResource source) {
        if (source.getCapacity() != null) {
            Capacity capacity = new Capacity();
            capacityMapper.map(source.getCapacity(), capacity);
        }
    }
}
