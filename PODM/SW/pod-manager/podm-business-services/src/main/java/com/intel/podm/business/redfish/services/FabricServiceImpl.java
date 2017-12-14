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
import com.intel.podm.business.dto.FabricDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.FabricDao;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRICS;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.ENDPOINTS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.SWITCHES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ZONES_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class FabricServiceImpl implements ReaderService<FabricDto> {
    @Inject
    EntityTreeTraverser traverser;

    @Inject
    FabricDao fabricDao;

    @Inject
    EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        return new CollectionDto(FABRICS, fabricDao.getAllFabricIds());
    }

    @Transactional(REQUIRED)
    @Override
    public FabricDto getResource(Context context) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(context);

        FabricDto dto = (FabricDto) entityToDtoMapper.map(fabric);
        dto.setZones(singletonContextOf(context, ZONES_RESOURCE_NAME));
        dto.setEndpoints(singletonContextOf(context, ENDPOINTS_RESOURCE_NAME));
        dto.setSwitches(singletonContextOf(context, SWITCHES_RESOURCE_NAME));
        return dto;
    }
}
