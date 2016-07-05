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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.SimpleStorageDto;
import com.intel.podm.business.dto.redfish.SimpleStorageDto.SimpleStorageDeviceDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.SimpleStorageService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SIMPLE_STORAGE;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class SimpleStorageServiceImpl implements SimpleStorageService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getSimpleStorageCollection(Context context) throws EntityNotFoundException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(context);
        return new CollectionDto(SIMPLE_STORAGE, getAsIdList(system.getSimpleStorages()));
    }

    @Override
    public SimpleStorageDto getSimpleStorage(Context context) throws EntityNotFoundException {
        SimpleStorage simpleStorage = (SimpleStorage) traverser.traverse(context);
        return SimpleStorageDto.newBuilder()
                .id(simpleStorage.getId().toString())
                .name(simpleStorage.getName())
                .description(simpleStorage.getDescription())
                .status(simpleStorage.getStatus())
                .uefiDevicePath(simpleStorage.getUefiDevicePath())
                .devices(getDevices(simpleStorage))
                .context(context)
                .build();
    }

    private List<SimpleStorageDeviceDto> getDevices(SimpleStorage simpleStorage) {
        return simpleStorage.getDevices().stream()
                .map(simpleStorageDevice -> new SimpleStorageDeviceDto(
                        simpleStorageDevice.getName(),
                        simpleStorageDevice.getManufacturer(),
                        simpleStorageDevice.getModel(),
                        simpleStorageDevice.getStatus(),
                        simpleStorageDevice.getCapacityBytes()
                )).collect(toList());
    }
}
