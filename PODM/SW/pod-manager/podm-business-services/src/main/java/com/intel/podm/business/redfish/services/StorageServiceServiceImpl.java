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
import com.intel.podm.business.dto.redfish.ServiceDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SERVICES;
import static com.intel.podm.business.redfish.ContextCollections.asManagerContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class StorageServiceServiceImpl implements ReaderService<ServiceDto> {
    @Inject
    private GenericDao genericDao;

    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) {
        List<StorageService> storageServices = genericDao.findAll(StorageService.class);
        return new CollectionDto(SERVICES, getAsIdSet(storageServices));
    }

    @Override
    public ServiceDto getResource(Context context) throws ContextResolvingException {
        StorageService service = (StorageService) traverser.traverse(context);
        return toServiceDto(service);
    }

    private ServiceDto toServiceDto(StorageService storageService) {
        return ServiceDto.newBuilder()
            .id(storageService.getId().toString())
            .name(storageService.getName())
            .description(storageService.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(storageService.getService(), storageService.getUnknownOems()))
            .status(storageService.getStatus())
            .managedBy(asManagerContexts(storageService.getManagers()))
            .build();
    }
}
