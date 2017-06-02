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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.StorageDto;
import com.intel.podm.business.dto.redfish.attributes.StorageControllerDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.StorageControllerDtoHelper;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.STORAGE;
import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static java.util.Comparator.comparingLong;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class StorageServiceImpl implements ReaderService<StorageDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private StorageControllerDtoHelper storageControllerDtoHelper;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(context);
        return new CollectionDto(STORAGE, getAsIdSet(system.getStorages()));
    }

    @Override
    public StorageDto getResource(Context context) throws ContextResolvingException {
        Storage storage = (Storage) traverser.traverse(context);
        return StorageDto.newBuilder()
            .id(storage.getId().toString())
            .name(storage.getName())
            .description(storage.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(storage.getService(), storage.getUnknownOems()))
            .enclosures(asChassisContexts(storage.getComputerSystem().getChassis()))
            .drives(asDriveContexts(storage.getDrives()))
            .storageControllers(mergeDataForStorageController(storage.getAdapters(), storage.getStorageControllers()))
            .status(storage.getStatus())
            .build();
    }

    private List<StorageControllerDto> mergeDataForStorageController(Set<StorageController> adapters,
                                                                     Set<StorageController> storageControllers) {
        List<StorageControllerDto> mergedCollections = adapters.stream()
            .sorted(comparingLong(Entity::getPrimaryKey))
            .map(adapter -> storageControllerDtoHelper.createStorageControllerDto(adapter))
            .collect(toList());

        mergedCollections.addAll(storageControllers.stream()
            .sorted(comparingLong(Entity::getPrimaryKey))
            .map(storageController -> storageControllerDtoHelper.createStorageControllerDto(storageController))
            .collect(toList()));

        return mergedCollections;
    }
}
