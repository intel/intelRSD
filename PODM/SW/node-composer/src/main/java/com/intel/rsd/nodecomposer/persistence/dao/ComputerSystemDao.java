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

package com.intel.rsd.nodecomposer.persistence.dao;

import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.Manager;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.Switch;
import org.springframework.stereotype.Component;

import javax.persistence.TypedQuery;
import java.util.Collection;
import java.util.List;
import java.util.Set;
import java.util.stream.Stream;

import static com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem.GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE;
import static com.intel.rsd.nodecomposer.types.PciePortType.UPSTREAM_PORT;
import static com.intel.rsd.nodecomposer.types.Protocol.PCIE;
import static com.intel.rsd.nodecomposer.types.SystemType.PHYSICAL;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.containsAny;

@Component
public class ComputerSystemDao extends Dao<ComputerSystem> {

    public List<ComputerSystem> getComputerSystemsPossibleToAllocate() {
        TypedQuery<ComputerSystem> query = entityManager.createNamedQuery(GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE, ComputerSystem.class);
        query.setParameter("allocated", false);
        query.setParameter("systemType", PHYSICAL);
        return query.getResultList();
    }

    public Set<ComputerSystem> getPhysicalComputerSystemsAchievableFromVirtualSystem(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "ComputerSystem can not be null.");

        List<String> pcieConnectionIds = getPcieConnectionIdsRelatedWithVirtualFpgaSystem(computerSystem);

        return getComputerSystemsPossibleToAllocate().stream()
            .filter(availableSystem -> containsAny(availableSystem.getPcieConnectionIds(), pcieConnectionIds))
            .collect(toSet());
    }

    private List<String> getPcieConnectionIdsRelatedWithVirtualFpgaSystem(ComputerSystem computerSystem) {
        Collection<Switch> switchesConnectedToComputerSystem = getSwitchesConnectedToComputerSystem(computerSystem);

        return getUpstreamPorts(switchesConnectedToComputerSystem)
            .map(Port::getPcieConnectionIds)
            .flatMap(Collection::stream)
            .collect(toList());
    }

    private Stream<Port> getUpstreamPorts(Collection<Switch> switches) {
        return switches.stream()
            .map(Switch::getPorts)
            .flatMap(Collection::stream)
            .filter(port -> UPSTREAM_PORT.equals(port.getPortType()));
    }

    private Collection<Switch> getSwitchesConnectedToComputerSystem(ComputerSystem computerSystem) {
        return computerSystem.getManagers().stream()
            .map(Manager::getFabrics)
            .flatMap(Collection::stream)
            .filter(fabric -> PCIE.equals(fabric.getFabricType()))
            .map(Fabric::getSwitches)
            .flatMap(Collection::stream)
            .collect(toSet());
    }
}
