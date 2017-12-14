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

package com.intel.podm.business.redfish.services.aggregation;

import com.intel.podm.business.dto.StorageControllerDto;
import com.intel.podm.business.entities.dao.StorageDao;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import static java.util.stream.Collectors.toList;
import static java.util.stream.Stream.concat;

@Dependent
public class StorageControllerCollectionMerger {
    @Inject
    private StorageControllerMerger storageControllerMerger;

    @Inject
    private StorageDao storageDao;

    public List<StorageControllerDto> getMergedStorageControllerCollection(Storage baseStorage) {
        List<Storage> complementaryStorages = storageDao.findComplementaryStorages(baseStorage);
        Map<String, StorageController> uniqueControllers = new LinkedHashMap<>();

        concat(
            concat(baseStorage.getStorageControllers().stream(), baseStorage.getAdapters().stream()),
            concat(
                complementaryStorages.stream().flatMap(storage -> storage.getStorageControllers().stream()),
                complementaryStorages.stream().flatMap(storage -> storage.getAdapters().stream())
            )
        ).forEach(controller -> uniqueControllers.put(controller.getMultiSourceDiscriminator(), controller));

        return uniqueControllers.values().stream()
            .map(storageController -> storageControllerMerger.toDto(storageController))
            .sorted()
            .collect(toList());
    }
}
