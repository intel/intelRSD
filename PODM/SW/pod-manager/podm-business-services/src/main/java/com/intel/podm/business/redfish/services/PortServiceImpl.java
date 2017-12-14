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
import com.intel.podm.business.dto.PortDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCH_PORTS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.PORT_METRICS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class PortServiceImpl implements ReaderService<PortDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context switchContext) throws ContextResolvingException {
        Switch fabricSwitch = (Switch) traverser.traverse(switchContext);
        return new CollectionDto(FABRIC_SWITCH_PORTS, getAsIdSet(fabricSwitch.getPorts()));
    }

    @Transactional(REQUIRED)
    @Override
    public PortDto getResource(Context portContext) throws ContextResolvingException {
        Port port = (Port) traverser.traverse(portContext);

        PortDto dto = (PortDto) entityToDtoMapper.map(port);
        dto.getActions().getReset().setTarget(singletonContextOf(portContext, "Actions/Port.Reset"));

        if (port.getPortMetrics() != null) {
            dto.getOem().getRackScale().setMetrics(singletonContextOf(portContext, PORT_METRICS_RESOURCE_NAME));
        }

        return dto;
    }
}
