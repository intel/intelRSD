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
import com.intel.podm.business.dto.redfish.ZoneDto;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ZONES;
import static com.intel.podm.business.redfish.ContextCollections.asEndpointContexts;
import static com.intel.podm.business.redfish.ContextCollections.asFabricSwitchContexts;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ZoneServiceImpl implements ReaderService<ZoneDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context fabricContext) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(fabricContext);
        return new CollectionDto(ZONES, getAsIdSet(fabric.getZones()));
    }

    @Override
    public ZoneDto getResource(Context context) throws ContextResolvingException {
        Zone zone = (Zone) traverser.traverse(context);
        return map(zone);
    }

    private ZoneDto map(Zone zone) {
        return ZoneDto.newBuilder()
            .id(zone.getId().toString())
            .name(zone.getName())
            .description(zone.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(zone.getService(), zone.getUnknownOems()))
            .involvedSwitches(asFabricSwitchContexts(zone.getSwitches()))
            .endpoints(asEndpointContexts(zone.getEndpoints()))
            .status(zone.getStatus())
            .build();
    }
}
