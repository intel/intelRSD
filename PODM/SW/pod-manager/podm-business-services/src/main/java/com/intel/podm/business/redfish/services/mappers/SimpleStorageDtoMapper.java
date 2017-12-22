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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.SimpleStorageDto;
import com.intel.podm.business.dto.SimpleStorageDto.SimpleStorageDeviceDto;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.SimpleStorageDevice;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.mappers.BrilliantMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

import static com.intel.podm.business.services.OemSerializeHelper.oemDtoToJsonNode;
import static java.util.Optional.ofNullable;

@Dependent
class SimpleStorageDtoMapper extends DtoMapper<SimpleStorage, SimpleStorageDto> {
    @Inject
    private SimpleStorageDeviceMapper simpleStorageDeviceMapper;

    SimpleStorageDtoMapper() {
        super(SimpleStorage.class, SimpleStorageDto.class);
        this.ignoredProperties("devices", "status");
    }

    @Override
    protected void performNotAutomatedMapping(SimpleStorage source, SimpleStorageDto target) {
        super.performNotAutomatedMapping(source, target);

        Map<String, SimpleStorageDeviceDto> deviceMap = new HashMap<>();
        for (SimpleStorageDevice simpleStorageDevice : source.getDevices()) {
            SimpleStorageDeviceDto deviceDto = new SimpleStorageDeviceDto();
            simpleStorageDeviceMapper.map(simpleStorageDevice, deviceDto);
            deviceMap.put(deviceDto.getName(), deviceDto);
        }
        target.setDevices(new ArrayList<>(deviceMap.values()));

        if (!source.isComplementary()) {
            ofNullable(source.getStatus()).ifPresent(target::setStatus);
        }
    }

    @Dependent
    private static class SimpleStorageDeviceMapper extends BrilliantMapper<SimpleStorageDevice, SimpleStorageDeviceDto> {
        @Inject
        private UnknownOemTranslator unknownOemTranslator;

        SimpleStorageDeviceMapper() {
            super(SimpleStorageDevice.class, SimpleStorageDeviceDto.class);
        }

        @Override
        protected void performNotAutomatedMapping(SimpleStorageDevice source, SimpleStorageDeviceDto target) {
            super.performNotAutomatedMapping(source, target);
            target.setOem(oemDtoToJsonNode(unknownOemTranslator.translateStringOemToDto(source.getSimpleStorage().getService(), source.getOem())));
        }
    }
}
