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
import com.intel.podm.business.dto.redfish.FabricDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRICS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class FabricServiceImpl implements ReaderService<FabricDto> {
    @Inject
    EntityTreeTraverser traverser;

    @Inject
    GenericDao genericDao;

    @Inject
    UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        List<Fabric> fabricsCollection = genericDao.findAll(Fabric.class);
        return new CollectionDto(FABRICS, getAsIdSet(fabricsCollection));
    }

    @Override
    public FabricDto getResource(Context context) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(context);
        return map(fabric);
    }

    public FabricDto map(Fabric fabric) throws ContextResolvingException {
        return FabricDto.newBuilder()
            .id(fabric.getId().toString())
            .name(fabric.getName())
            .description(fabric.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(fabric.getService(), fabric.getUnknownOems()))
            .status(fabric.getStatus())
            .maxZones(fabric.getMaxZones())
            .fabricType(fabric.getFabricType())
            .build();
    }
}
