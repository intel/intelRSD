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

import com.intel.rsd.nodecomposer.externalservices.resources.redfish.StoragePoolResource;
import com.intel.rsd.nodecomposer.mappers.EntityMapper;
import com.intel.rsd.nodecomposer.persistence.redfish.StoragePool;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Capacity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class StoragePoolMapper extends EntityMapper<StoragePoolResource, StoragePool> {
    private final CapacityMapper capacityMapper;

    @Autowired
    protected StoragePoolMapper(CapacityMapper capacityMapper) {
        super(StoragePoolResource.class, StoragePool.class);
        this.capacityMapper = capacityMapper;
    }

    @Override
    protected void performNotAutomatedMapping(StoragePoolResource source, StoragePool target) {
        super.performNotAutomatedMapping(source, target);

        mapCapacity(source);
    }

    private void mapCapacity(StoragePoolResource source) {
        if (source.getCapacity() != null) {
            Capacity capacity = new Capacity();
            capacityMapper.map(source.getCapacity(), capacity);
        }
    }
}
