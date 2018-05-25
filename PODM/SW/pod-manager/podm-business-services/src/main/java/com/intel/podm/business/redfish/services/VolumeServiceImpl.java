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
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.VolumeDto;
import com.intel.podm.business.entities.redfish.StoragePool;
import com.intel.podm.business.entities.redfish.StorageService;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.VOLUME;
import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEndpointContexts;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ActionsResourceNames.ACTIONS_VOLUME_INITIALIZE;
import static com.intel.podm.common.types.redfish.ResourceNames.VOLUME_METRICS_RESOURCE_NAME;
import static java.lang.Boolean.TRUE;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class VolumeServiceImpl implements ReaderService<VolumeDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        switch (context.getType()) {
            case STORAGE_POOL:
                StoragePool storagePool = (StoragePool) traverser.traverse(context);
                return new CollectionDto(VOLUME, storagePool.getAllocatedVolumes().stream().map(Contexts::toContext).sorted().collect(toList()));
            case STORAGE_SERVICE:
                StorageService storageService = (StorageService) traverser.traverse(context);
                return new CollectionDto(VOLUME, storageService.getVolumes().stream().map(Contexts::toContext).sorted().collect(toList()));
            default:
                throw new ContextResolvingException("Unsupported parent context of Volume Collection", context, null);
        }
    }

    @Transactional(REQUIRED)
    @Override
    public VolumeDto getResource(Context context) throws ContextResolvingException {
        Volume volume = (Volume) traverser.traverse(context);
        VolumeDto dto = (VolumeDto) entityToDtoMapper.map(volume);
        VolumeDto.Links links = dto.getLinks();
        links.setDrives(asDriveContexts(volume.getDrives()));
        links.getOem().getRackScaleOem().setEndpoints(asEndpointContexts(volume.getEndpoints()));
        if (Objects.equals(volume.getInitializeActionSupported(), TRUE)) {
            VolumeDto.Actions actions = dto.getActions();
            actions.getInitializeAction().setTarget(singletonContextOf(context, ACTIONS_VOLUME_INITIALIZE));
            actions.getInitializeAction().setAllowableValues(dto.getActions().getInitializeAction().getAllowableValues());
        }

        if (volume.getMetrics() != null) {
            dto.getOem().getRackScaleOem().setVolumeMetrics(singletonContextOf(context, VOLUME_METRICS_RESOURCE_NAME));
        }
        return dto;
    }
}
