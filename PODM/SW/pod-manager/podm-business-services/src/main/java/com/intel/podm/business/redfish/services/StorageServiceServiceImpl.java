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
import com.intel.podm.business.dto.StorageServiceDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.StorageServiceDao;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SERVICES;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.LOGICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PHYSICAL_DRIVES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.REMOTE_TARGETS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
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
        return new CollectionDto(SERVICES, storageServiceDao.getAllStorageServiceIds());
    }

    @Transactional(REQUIRED)
    @Override
    public StorageServiceDto getResource(Context context) throws ContextResolvingException {
        StorageService storageService = (StorageService) traverser.traverse(context);

        StorageServiceDto dto = (StorageServiceDto) entityToDtoMapper.map(storageService);
        dto.setRemoteTargets(singletonContextOf(context, REMOTE_TARGETS_RESOURCE_NAME));
        dto.setLogicalDrives(singletonContextOf(context, LOGICAL_DRIVES_RESOURCE_NAME));
        dto.setDrives(singletonContextOf(context, PHYSICAL_DRIVES_RESOURCE_NAME));
        dto.getLinks().getManagedBy().addAll(asManagerContexts(storageService.getManagers()));
        return dto;
    }
}
