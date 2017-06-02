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
import com.intel.podm.business.dto.redfish.VlanNetworkInterfaceDto;
import com.intel.podm.business.entities.redfish.EthernetSwitchPortVlan;
import com.intel.podm.business.entities.redfish.base.VlanPossessor;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.RedfishVlanNetworkInterface;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_SWITCH_PORT_VLANS;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static java.util.EnumSet.of;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class VlanNetworkInterfaceReaderServiceImpl implements ReaderService<RedfishVlanNetworkInterface> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        if (!of(ETHERNET_INTERFACE, ETHERNET_SWITCH_PORT).contains(context.getType())) {
            throw new ContextResolvingException(context);
        }

        VlanPossessor vlanPossessor = (VlanPossessor) traverser.traverse(context);
        return new CollectionDto(ETHERNET_SWITCH_PORT_VLANS, getAsIdSet(vlanPossessor.getEthernetSwitchPortVlans()));
    }

    @Override
    public RedfishVlanNetworkInterface getResource(Context ethernetSwitchPortVlan) throws ContextResolvingException {
        EthernetSwitchPortVlan portVlan = (EthernetSwitchPortVlan) traverser.traverse(ethernetSwitchPortVlan);
        return VlanNetworkInterfaceDto.newBuilder()
            .id(portVlan.getId().toString())
            .name(portVlan.getName())
            .description(portVlan.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(portVlan.getService(), portVlan.getUnknownOems()))
            .vlanEnable(portVlan.getVlanEnable())
            .tagged(portVlan.getTagged())
            .vlanId(portVlan.getVlanId())
            .status(portVlan.getStatus())
            .build();
    }
}
