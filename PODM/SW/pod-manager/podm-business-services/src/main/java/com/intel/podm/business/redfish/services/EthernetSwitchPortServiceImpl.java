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
import com.intel.podm.business.dto.EthernetSwitchPortDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.EthernetSwitch;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORTS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_PORT_METRICS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
class EthernetSwitchPortServiceImpl implements ReaderService<EthernetSwitchPortDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context ethernetSwitch) throws ContextResolvingException {
        EthernetSwitch aSwitch = (EthernetSwitch) traverser.traverse(ethernetSwitch);
        return new CollectionDto(ETHERNET_SWITCH_PORTS, getAsIdSet(aSwitch.getPorts()));
    }

    @Transactional(REQUIRED)
    @Override
    public EthernetSwitchPortDto getResource(Context context) throws ContextResolvingException {
        EthernetSwitchPort port = (EthernetSwitchPort) traverser.traverse(context);

        EthernetSwitchPortDto dto = (EthernetSwitchPortDto) entityToDtoMapper.map(port);
        dto.setVlans(singletonContextOf(context, ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME));
        dto.setStaticMacs(singletonContextOf(context, ETHERNET_SWITCH_STATIC_MACS_RESOURCE_NAME));
        if (port.getMetrics() != null) {
            dto.setMetrics(singletonContextOf(context, ETHERNET_SWITCH_PORT_METRICS_RESOURCE_NAME));
        }
        return dto;
    }
}
