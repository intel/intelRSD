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
import com.intel.podm.business.dto.SwitchDto;
import com.intel.podm.business.dto.actions.ResetActionDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.Fabric;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.FABRIC_SWITCHES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.PORTS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class SwitchServiceImpl implements ReaderService<SwitchDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context fabricContext) throws ContextResolvingException {
        Fabric fabric = (Fabric) traverser.traverse(fabricContext);
        return new CollectionDto(FABRIC_SWITCHES, getAsIdSet(fabric.getSwitches()));
    }

    @Transactional(REQUIRED)
    @Override
    public SwitchDto getResource(Context context) throws ContextResolvingException {
        Switch fabricSwitch = (Switch) traverser.traverse(context);

        SwitchDto dto = (SwitchDto) entityToDtoMapper.map(fabricSwitch);
        dto.setPorts(singletonContextOf(context, PORTS_RESOURCE_NAME));

        ResetActionDto resetAction = dto.getActions().getReset();
        resetAction.setAllowableResetTypes(fabricSwitch.getAllowableResetTypes());
        resetAction.setTarget(singletonContextOf(context, "Actions/Switch.Reset"));
        return dto;
    }
}
