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
import com.intel.podm.business.dto.VlanNetworkInterfaceDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.EthernetSwitchPort;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.MultiSourceEntityTreeTraverser;
import com.intel.podm.business.redfish.services.mappers.EntityToDtoMapper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORT_VLANS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class VlanNetworkInterfaceReaderServiceImpl implements ReaderService<VlanNetworkInterfaceDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private MultiSourceEntityTreeTraverser multiTraverser;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        ContextType type = context.getType();
        if (Objects.equals(ETHERNET_SWITCH_PORT, type)) {
            EthernetSwitchPort port = (EthernetSwitchPort) traverser.traverse(context);
            return new CollectionDto(ETHERNET_SWITCH_PORT_VLANS, getAsIdSet(port.getEthernetSwitchPortVlans()));
        } else if (Objects.equals(ETHERNET_INTERFACE, type)) {
            EthernetInterface iface = (EthernetInterface) multiTraverser.traverse(context);
            return new CollectionDto(ETHERNET_SWITCH_PORT_VLANS, getAsIdSet(iface.getEthernetSwitchPortVlans()));
        }
        throw new ContextResolvingException(context);
    }

    @Transactional(REQUIRED)
    @Override
    public VlanNetworkInterfaceDto getResource(Context context) throws ContextResolvingException {
        EthernetSwitchPortVlan vlan = (EthernetSwitchPortVlan) traverser.traverse(context);
        return (VlanNetworkInterfaceDto) entityToDtoMapper.map(vlan);
    }
}
