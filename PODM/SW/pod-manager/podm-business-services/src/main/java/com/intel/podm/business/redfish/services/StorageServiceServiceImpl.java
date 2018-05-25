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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.StorageServiceDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.StorageServiceDao;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SERVICE;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.STORAGE_SERVICE;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.STORAGE_POOL_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.VOLUMES_RESOURCE_NAME;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class StorageServiceServiceImpl implements ReaderService<StorageServiceDto> {
    @Inject
    private StorageServiceDao storageServiceDao;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) {
        List<Context> contexts = storageServiceDao.getAllStorageServiceIds().stream().map(id -> contextOf(id, STORAGE_SERVICE)).sorted().collect(toList());
        return new CollectionDto(SERVICE, contexts);
    }

    @Transactional(REQUIRED)
    @Override
    public StorageServiceDto getResource(Context context) throws ContextResolvingException {
        StorageService storageService = (StorageService) traverser.traverse(context);

        StorageServiceDto dto = (StorageServiceDto) entityToDtoMapper.map(storageService);
        dto.setStoragePools(singletonContextOf(context, STORAGE_POOL_RESOURCE_NAME));
        dto.setVolumes(singletonContextOf(context, VOLUMES_RESOURCE_NAME));
        dto.setDrives(singletonContextOf(context, DRIVES_RESOURCE_NAME));
        setLinkToEndpoints(storageService, dto);
        dto.getLinks().getOem().getRackScaleOem().getManagedBy().addAll(asManagerContexts(storageService.getManagers()));
        return dto;
    }

    private void setLinkToEndpoints(StorageService storageService, StorageServiceDto dto) {
        Optional<Fabric> optionalFabric = retrieveFabricFromStorageService(storageService);
        optionalFabric.ifPresent(fabric -> dto.setEndpoints(singletonContextOf(toContext(fabric), ENDPOINTS_RESOURCE_NAME)));
    }

    private Optional<Fabric> retrieveFabricFromStorageService(StorageService storageService) {
        return storageService.getService().getOwnedEntities().stream()
            .filter(entity -> entity instanceof Fabric)
            .map(Fabric.class::cast)
            .findFirst();
    }
}
