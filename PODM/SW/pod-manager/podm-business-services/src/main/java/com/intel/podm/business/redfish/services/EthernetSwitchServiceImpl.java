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
import com.intel.podm.business.dto.EthernetSwitchDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.dao.EthernetSwitchDao;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCHES;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.ACLS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_METRICS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.PORTS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class EthernetSwitchServiceImpl implements ReaderService<EthernetSwitchDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EthernetSwitchDao ethernetSwitchDao;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context serviceRootContext) throws ContextResolvingException {
        return new CollectionDto(ETHERNET_SWITCHES, ethernetSwitchDao.getAllEthernetSwitchIds());
    }

    @Transactional(REQUIRED)
    @Override
    public EthernetSwitchDto getResource(Context ethernetSwitchContext) throws ContextResolvingException {
        EthernetSwitch ethernetSwitch = (EthernetSwitch) traverser.traverse(ethernetSwitchContext);

        EthernetSwitchDto target = (EthernetSwitchDto) entityToDtoMapper.map(ethernetSwitch);
        target.setAcls(singletonContextOf(ethernetSwitchContext, ACLS_RESOURCE_NAME));
        target.setPorts(singletonContextOf(ethernetSwitchContext, PORTS_RESOURCE_NAME));
        if (ethernetSwitch.getEthernetSwitchMetrics() != null) {
            target.setMetrics(singletonContextOf(ethernetSwitchContext, ETHERNET_SWITCH_METRICS_RESOURCE_NAME));
        }
        return target;
    }
}
