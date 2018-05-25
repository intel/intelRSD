/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.business.entities.dao.StorageDao;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.function.Function;

import static java.util.Collections.emptyList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class StorageSubResourcesFinder {
    @Inject
    private StorageDao storageDao;

    @Transactional(MANDATORY)
    public Collection<? extends DiscoverableEntity> getUniqueSubResourcesOfClass(Storage baseEntity, Class<? extends DiscoverableEntity> subResourceClass) {
        List<Storage> complementaryStorages = storageDao.findComplementaryStorages(baseEntity);
        if (!Objects.equals(subResourceClass, StorageController.class)) {
            return emptyList();
        }

        Map<String, StorageController> storageControllers = getStringMap(baseEntity, complementaryStorages, Storage::getStorageControllers);

        // backward compatibility for Adapter resource
        storageControllers.putAll(getStringMap(baseEntity, complementaryStorages, Storage::getAdapters));

        return storageControllers.values();
    }

    private <T extends MultiSourceResource> Map<String, T> getStringMap(Storage storage, List<Storage> complementaryStorages,
                                                                        Function<Storage, Collection<T>> func) {
        Map<String, T> map = new HashMap<>();
        for (T item : func.apply(storage)) {
            map.put(item.getMultiSourceDiscriminator(), item);
        }

        for (Storage complementaryStorage : complementaryStorages) {
            for (T complementaryItem : func.apply(complementaryStorage)) {
                map.putIfAbsent(complementaryItem.getMultiSourceDiscriminator(), complementaryItem);
            }
        }
        return map;
    }
}
