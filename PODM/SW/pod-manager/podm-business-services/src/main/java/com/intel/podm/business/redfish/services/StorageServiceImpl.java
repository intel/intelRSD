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
import com.intel.podm.business.dto.StorageControllerDto;
import com.intel.podm.business.dto.StorageDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.ComputerSystemSubResourcesFinder;
import com.intel.podm.business.redfish.services.aggregation.MultiSourceEntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.StorageControllerCollectionMerger;
import com.intel.podm.business.redfish.services.aggregation.StorageMerger;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.STORAGE;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class StorageServiceImpl implements ReaderService<StorageDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private MultiSourceEntityTreeTraverser multiTraverser;

    @Inject
    private ComputerSystemSubResourcesFinder computerSystemSubResourcesFinder;

    @Inject
    private StorageMerger storageMerger;

    @Inject
    private StorageControllerCollectionMerger storageControllerCollectionMerger;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        ComputerSystem system = (ComputerSystem) traverser.traverse(context);

        // Multi-source resources sanity check
        if (system.isComplementary()) {
            throw new ContextResolvingException("Specified resource is not a primary resource representation!", context, null);
        }

        return new CollectionDto(STORAGE, getAsIdSet(computerSystemSubResourcesFinder.getUniqueSubResourcesOfClass(system, Storage.class)));
    }

    @Transactional(REQUIRED)
    @Override
    public StorageDto getResource(Context context) throws ContextResolvingException {
        Storage storage = (Storage) multiTraverser.traverse(context);
        StorageDto storageDto = storageMerger.toDto(storage);
        storageDto.setId(storage.getId().toString());

        List<StorageControllerDto> controllers = storageControllerCollectionMerger.getMergedStorageControllerCollection(storage);
        ODataId oDataId = asOdataId(context);
        for (StorageControllerDto storageControllerDto : controllers) {
            storageControllerDto.setoDataId(format("%s#/StorageControllers/%s", oDataId, controllers.indexOf(storageControllerDto)));
        }
        storageDto.setStorageControllers(controllers);
        return storageDto;
    }
}
