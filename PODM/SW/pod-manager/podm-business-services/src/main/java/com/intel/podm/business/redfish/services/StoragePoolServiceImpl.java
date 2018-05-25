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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.StoragePoolDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.StoragePool;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.STORAGE_POOL;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.NotMappedResourceNames.ALLOCATED_POOLS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.NotMappedResourceNames.ALLOCATED_VOLUMES_RESOURCE_NAME;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class StoragePoolServiceImpl implements ReaderService<StoragePoolDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        switch (context.getType()) {
            case STORAGE_POOL:
                StoragePool storagePool = (StoragePool) traverser.traverse(context);
                return new CollectionDto(STORAGE_POOL, storagePool.getAllocatedPools().stream().map(Contexts::toContext).sorted().collect(toList()));
            case STORAGE_SERVICE:
                StorageService storageService = (StorageService) traverser.traverse(context);
                return new CollectionDto(STORAGE_POOL, storageService.getStoragePools().stream().map(Contexts::toContext).sorted().collect(toList()));
            default:
                throw new ContextResolvingException("Unsupported parent context of StoragePool Collection", context, null);
        }
    }

    @Override
    public StoragePoolDto getResource(Context context) throws ContextResolvingException {
        StoragePool storagePool = (StoragePool) traverser.traverse(context);

        StoragePoolDto storagePoolDto = (StoragePoolDto) entityToDtoMapper.map(storagePool);
        storagePoolDto.setAllocatedVolumes(singletonContextOf(context, ALLOCATED_VOLUMES_RESOURCE_NAME));
        storagePoolDto.setAllocatedPools(singletonContextOf(context, ALLOCATED_POOLS_RESOURCE_NAME));

        return storagePoolDto;
    }
}
