/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.business.dto.EthernetSwitchAclDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchAcl;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_ACLS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.RULES_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
public class EthernetSwitchAclServiceImpl implements ReaderService<EthernetSwitchAclDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        EthernetSwitch ethernetSwitch = (EthernetSwitch) traverser.traverse(context);
        return new CollectionDto(ETHERNET_SWITCH_ACLS, getAsIdSet(ethernetSwitch.getAcls()));
    }

    @Transactional(REQUIRED)
    @Override
    public EthernetSwitchAclDto getResource(Context context) throws ContextResolvingException {
        EthernetSwitchAcl acl = (EthernetSwitchAcl) traverser.traverse(context);

        EthernetSwitchAclDto dto = (EthernetSwitchAclDto) entityToDtoMapper.map(acl);
        dto.setRules(singletonContextOf(context, RULES_RESOURCE_NAME));

        EthernetSwitchAclDto.Actions actions = dto.getActions();
        actions.getBind().setTarget(singletonContextOf(context, "Actions/EthernetSwitchACL.Bind"));
        actions.getUnbind().setTarget(singletonContextOf(context, "Actions/EthernetSwitchACL.Unbind"));

        return dto;
    }
}
