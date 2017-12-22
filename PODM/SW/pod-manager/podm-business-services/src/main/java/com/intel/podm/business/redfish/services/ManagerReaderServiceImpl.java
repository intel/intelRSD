/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.dto.ManagerDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.ManagerDao;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.MANAGERS;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_INTERFACES_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.NETWORK_PROTOCOL_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class ManagerReaderServiceImpl implements ReaderService<ManagerDto> {
    @Inject
    EntityTreeTraverser traverser;

    @Inject
    EntityToDtoMapper entityToDtoMapper;

    @Inject
    private ManagerDao managerDao;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        return new CollectionDto(MANAGERS, managerDao.getAllManagerIds());
    }

    @Transactional(REQUIRED)
    @Override
    public ManagerDto getResource(Context context) throws ContextResolvingException {
        Manager manager = (Manager) traverser.traverse(context);

        ManagerDto dto = (ManagerDto) entityToDtoMapper.map(manager);
        dto.setEthernetInterfaces(singletonContextOf(context, ETHERNET_INTERFACES_RESOURCE_NAME));
        if (manager.getNetworkProtocol() != null) {
            dto.setNetworkProtocol(singletonContextOf(context, NETWORK_PROTOCOL_RESOURCE_NAME));
        }
        return dto;
    }
}
