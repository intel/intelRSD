/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.dto.EthernetInterfaceDto;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.aggregation.ComputerSystemSubResourcesFinder;
import com.intel.podm.business.redfish.services.aggregation.EthernetInterfaceMerger;
import com.intel.podm.business.redfish.services.aggregation.MultiSourceEntityTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.EnumSet;
import java.util.List;
import java.util.Objects;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.ETHERNET_INTERFACE;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.SingletonContext.singletonContextOf;
import static com.intel.podm.common.types.redfish.ResourceNames.ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class EthernetInterfaceServiceImpl implements ReaderService<EthernetInterfaceDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private MultiSourceEntityTreeTraverser multiTraverser;

    @Inject
    private ComputerSystemSubResourcesFinder computerSystemSubResourcesFinder;

    @Inject
    private EthernetInterfaceMerger ethernetInterfaceMerger;

    @Transactional(REQUIRED)
    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        if (Objects.equals(MANAGER, context.getType())) {
            Manager manager = (Manager) traverser.traverse(context);
            List<Context> contexts = manager.getEthernetInterfaces().stream().map(Contexts::toContext).sorted().collect(toList());
            return new CollectionDto(ETHERNET_INTERFACE, contexts);
        }

        if (Objects.equals(COMPUTER_SYSTEM, context.getType())) {
            ComputerSystem system = (ComputerSystem) traverser.traverse(context);

            // Multi-source resources sanity check
            if (system.isComplementary()) {
                throw new ContextResolvingException("Specified resource is not a primary resource representation!", context, null);
            }

            List<Context> contexts = computerSystemSubResourcesFinder.getUniqueSubResourcesOfClass(system, EthernetInterface.class).stream()
                .map(Contexts::toContext).sorted().collect(toList());
            return new CollectionDto(ETHERNET_INTERFACE, contexts);
        }

        throw new ContextResolvingException(context);
    }

    @Transactional(REQUIRED)
    @Override
    public EthernetInterfaceDto getResource(Context context) throws ContextResolvingException {
        EthernetInterface ethernetInterface = (EthernetInterface) multiTraverser.traverse(context);
        if (EnumSet.of(MANAGER, COMPUTER_SYSTEM).contains(context.getParent().getType())) {
            EthernetInterfaceDto dto = ethernetInterfaceMerger.toDto(ethernetInterface);
            dto.setVlans(singletonContextOf(context, ETHERNET_SWITCH_PORT_VLANS_RESOURCE_NAME));
            return dto;
        }
        throw new ContextResolvingException(context);
    }
}
