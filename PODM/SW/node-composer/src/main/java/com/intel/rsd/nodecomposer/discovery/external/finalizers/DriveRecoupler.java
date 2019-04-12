/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.dao.PciePortDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.collections.IterableHelper.single;
import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isNotEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
class DriveRecoupler extends Recoupler<Drive> {
    private final PciePortDao pciePortDao;

    @Autowired
    DriveRecoupler(PciePortDao pciePortDao) {
        this.pciePortDao = pciePortDao;
    }

    @Override
    protected void reattach(ComposedNode node, Collection<Drive> assets) {
        getDrivesConnectedWithEndpoint(assets).forEach(drive -> attachDriveToComposedNode(node, drive));
    }

    @Override
    protected boolean verify(ComposedNode node) {
        return node.getAssociatedDrivesUris().isEmpty()
            || composedNodeDrivesCollectionIsConsistent(node);
    }

    private boolean composedNodeDrivesCollectionIsConsistent(ComposedNode node) {
        return !node.getDrives().isEmpty()
            && allDrivesAreConnectedToNode(node)
            && allDrivesAreHealthyAndConnectedWithEndpoints(node);
    }

    private boolean allDrivesAreConnectedToNode(ComposedNode node) {
        return node.getAssociatedDrivesUris().size() == node.getDrives().size()
            && composedNodeContainsAllAssociatedDrives(node);
    }

    private boolean composedNodeContainsAllAssociatedDrives(ComposedNode node) {
        Set<URI> composedNodeDrivesUris = node.getDrives().stream()
            .filter(Drive::isAchievable)
            .map(drive -> drive.getUri().toUri())
            .collect(toSet());

        return composedNodeDrivesUris.containsAll(node.getAssociatedDrivesUris());
    }

    private boolean allDrivesAreHealthyAndConnectedWithEndpoints(ComposedNode composedNode) {
        return composedNode.getDrives().stream().allMatch(drive -> drive.isAvailable()
            && isNotEmpty(drive.getEntityConnections())
            && drive.getEntityConnections().stream()
            .noneMatch(connectedEntity -> isNull(connectedEntity.getEndpoint()) && TARGET.equals(connectedEntity.getEntityRole()))
        );
    }

    private List<Drive> getDrivesConnectedWithEndpoint(Collection<Drive> drives) {
        return drives.stream()
            .filter(drive -> !drive.getEntityConnections().isEmpty())
            .filter(drive -> single(drive.getEntityConnections()).getEndpoint() != null)
            .collect(toList());
    }

    private void attachDriveToComposedNode(ComposedNode composedNodesEligibleForRecovery, Drive drive) {
        Map<String, ComputerSystem> localPcieConnectionIds = getComputerSystemPcieConnectionIdMap(composedNodesEligibleForRecovery);

        pciePortDao.getUpstreamPortsByDiscoverableEntity(drive)
            .stream()
            .map(Port::getPcieConnectionIds)
            .flatMap(Collection::stream)
            .filter(localPcieConnectionIds::containsKey)
            .map(localPcieConnectionIds::get)
            .filter(computerSystem -> computerSystem.getComposedNode() != null)
            .map(ComputerSystem::getComposedNode)
            .filter(node -> Objects.equals(node, composedNodesEligibleForRecovery))
            .peek(node -> log.trace(format("Reattaching drive(%s) to the node(%s)", drive, node)))
            .forEach(node -> attachDrive(node, drive));
    }

    private Map<String, ComputerSystem> getComputerSystemPcieConnectionIdMap(ComposedNode composedNodesEligibleForRecovery) {
        Map<String, ComputerSystem> pcieConnectionIdToComputerSystemMap = new HashMap<>();
        ComputerSystem system = composedNodesEligibleForRecovery.getComputerSystem();
        if (system != null) {
            for (String cableId : system.getPcieConnectionIds()) {
                pcieConnectionIdToComputerSystemMap.put(cableId, system);
            }
        }
        return pcieConnectionIdToComputerSystemMap;
    }

    private void attachDrive(ComposedNode node, Drive drive) {
        node.attachAsset(drive);
    }
}
