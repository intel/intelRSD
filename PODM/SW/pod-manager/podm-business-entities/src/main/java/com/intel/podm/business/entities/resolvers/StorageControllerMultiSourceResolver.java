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

package com.intel.podm.business.entities.resolvers;

import com.intel.podm.business.entities.dao.StorageControllerDao;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.StorageController;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Optional;

import static com.intel.podm.common.types.DurableNameFormat.UUID;

@Dependent
public class StorageControllerMultiSourceResolver extends MultiSourceEntityResolver<StorageController> {
    private final StorageControllerDao storageControllerDao;

    @Inject
    private StorageMultiSourceResolver storageMultiSourceResolver;

    @Inject
    StorageControllerMultiSourceResolver(StorageControllerDao storageControllerDao) {
        super(StorageController.class);
        this.storageControllerDao = storageControllerDao;
    }

    @Override
    protected Optional<StorageController> findPrimaryEntity(StorageController complementaryStorageController) {
        return storageControllerDao.findPrimaryStorageController(complementaryStorageController);
    }

    @Override
    public String createMultiSourceDiscriminator(StorageController storageController) {
        String controllerDiscriminator = storageController.getSourceUri().toString();
        for (Identifier identifier : storageController.getIdentifiers()) {
            if (UUID.equals(identifier.getDurableNameFormat())) {
                controllerDiscriminator = identifier.getDurableName();
            }
        }

        return getStorageDiscriminatorWithFallback(storageController) + controllerDiscriminator;
    }

    private String getStorageDiscriminatorWithFallback(StorageController storageController) {
        Storage storage = storageController.getStorage();
        String storageDiscriminator = storage.getMultiSourceDiscriminator();
        if (storageDiscriminator == null) {
            storageDiscriminator = storageMultiSourceResolver.createMultiSourceDiscriminator(storage);
        }
        return storageDiscriminator;
    }
}
