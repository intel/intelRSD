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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.StoragePoolDto;
import com.intel.podm.business.dto.redfish.CapacitySourceDto;
import com.intel.podm.business.entities.redfish.CapacitySource;
import com.intel.podm.business.entities.redfish.StoragePool;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class StoragePoolDtoMapper extends DtoMapper<StoragePool, StoragePoolDto> {
    @Inject
    private CapacitySourcesDtoMapper capacitySourcesDtoMapper;

    StoragePoolDtoMapper() {
        super(StoragePool.class, StoragePoolDto.class);
        this.ignoredProperties("allocatedPools", "allocatedVolumes", "capacitySources");
    }

    @Override
    protected void performNotAutomatedMapping(StoragePool source, StoragePoolDto target) {
        super.performNotAutomatedMapping(source, target);

        mapCapacitySources(source, target);
    }

    private void mapCapacitySources(StoragePool source, StoragePoolDto target) {
        for (CapacitySource capacitySource : source.getCapacitySources()) {
            CapacitySourceDto dto = new CapacitySourceDto();
            capacitySourcesDtoMapper.map(capacitySource, dto);
            target.getCapacitySources().add(dto);
        }
    }
}
