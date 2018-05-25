/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.dao.PciePortDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

import static com.intel.podm.common.utils.IterableHelper.single;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;

@Dependent
class DriveRecoupler extends Recoupler<Drive> {
    @Inject
    Logger logger;

    @Inject
    private PciePortDao pciePortDao;

    @Override
    protected void reattach(ComposedNode node, Collection<Drive> assets) {
        getDrivesConnectedWithEndpoint(assets).forEach(drive -> attachDriveToComposedNode(node, drive));
    }

    @Override
    protected boolean verify(ComposedNode node) {
        return node.getDrives().size() == node.getNumberOfRequestedDrives();
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
            .peek(node -> logger.t(format("Reattaching drive(%s) to the node(%s)", drive, node)))
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
        node.addDrive(drive);
        drive.getMetadata().setAllocated(true);
    }
}
