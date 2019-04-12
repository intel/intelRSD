/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.mappers.ethernetinterface.EthernetInterfacesAllocationMapper;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.VlanNodeAssemblyTaskFactory;
import com.intel.rsd.nodecomposer.persistence.NonUniqueResultException;
import com.intel.rsd.nodecomposer.persistence.dao.EthernetSwitchPortDao;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetSwitchPort;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class EthernetInterfacesAllocator {
    private final EthernetInterfacesAllocationMapper mapper;
    private final VlanNodeAssemblyTaskFactory taskFactory;
    private final EthernetSwitchPortDao ethernetSwitchPortDao;

    @Autowired
    public EthernetInterfacesAllocator(EthernetInterfacesAllocationMapper mapper, VlanNodeAssemblyTaskFactory taskFactory,
                                       EthernetSwitchPortDao ethernetSwitchPortDao) {
        this.mapper = mapper;
        this.taskFactory = taskFactory;
        this.ethernetSwitchPortDao = ethernetSwitchPortDao;
    }

    @Transactional(REQUIRED)
    public List<NodeTask> createVlanAssemblyTasks(Collection<RequestedNode.EthernetInterface> requestedInterfaces,
                                                  Collection<EthernetInterface> availableInterfaces) {
        Map<EthernetInterface, RequestedNode.EthernetInterface> mappedInterfaces = mapper.map(requestedInterfaces, availableInterfaces);

        return mappedInterfaces.entrySet().stream()
            .filter(entry -> entry.getValue().getVlans().isPresent())
            .filter(entry -> {
                EthernetSwitchPort ethernetSwitchPort = null;
                try {
                    ethernetSwitchPort = ethernetSwitchPortDao.getEnabledAndHealthyEthernetSwitchPortByNeighborMac(entry.getKey().getMacAddress());
                } catch (NonUniqueResultException e) {
                    log.error(format("Could not use Ethernet Interface '%s' for allocation.", entry.getKey()), e);
                }

                return ethernetSwitchPort != null;
            })
            .map(entry -> taskFactory.createVlanCreationTask(entry.getKey(), entry.getValue()))
            .collect(toList());
    }
}
