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

package com.intel.podm.discovery;

import com.intel.podm.business.entities.dao.ComposedNodeDao;
import com.intel.podm.business.entities.dao.PciePortDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.RemoteTarget;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.ComposedNode.OFFLINE_CRITICAL_STATUS;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static com.intel.podm.common.types.Health.CRITICAL;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;
import static com.intel.podm.common.types.State.STANDBY_OFFLINE;
import static com.intel.podm.common.utils.Collections.filterByType;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static java.util.stream.Stream.of;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@SuppressWarnings({"checkstyle:MethodCount"})
public class ComposedNodeUpdater {
    @Inject
    ComposedNodeDao composedNodeDao;

    @Inject
    PciePortDao pciePortDao;

    @Inject
    Logger logger;

    @Transactional(MANDATORY)
    public void updateRelatedComposedNodes(Set<Entity> entities) {
        List<DiscoverableEntity> enabledAndHealthy = getEnabledAndHealthyDiscoverableEntities(entities);

        reattachComposedNodeResources(
            filterByType(enabledAndHealthy, ComputerSystem.class),
            filterByType(enabledAndHealthy, RemoteTarget.class),
            filterByType(enabledAndHealthy, Drive.class)
        );

        List<ComputerSystem> computerSystems = getNotEnabledAssetsOfType(entities, ComputerSystem.class);
        computerSystems.stream().map(ComputerSystem::getComposedNode).forEach(this::disableComposedNode);

        List<RemoteTarget> targets = getNotEnabledAssetsOfType(entities, RemoteTarget.class);
        targets.stream().map(RemoteTarget::getComposedNode).forEach(this::disableComposedNode);

        List<Drive> drives = getNotEnabledAssetsOfType(entities, Drive.class);
        drives.stream().map(Drive::getComposedNode).forEach(this::disableComposedNode);
    }

    private void reattachComposedNodeResources(Collection<ComputerSystem> computerSystems, Collection<RemoteTarget> targets, Collection<Drive> drives) {
        List<ComposedNode> composedNodesEligibleForRecovery = composedNodeDao.getComposedNodesEligibleForRecovery();
        reattachComputerSystems(composedNodesEligibleForRecovery, computerSystems);
        reattachRemoteTargets(composedNodesEligibleForRecovery, targets);
        reattachEndpoints(composedNodesEligibleForRecovery, drives);

        recoverComposedNodes(composedNodesEligibleForRecovery);
    }

    private void reattachEndpoints(List<ComposedNode> composedNodesEligibleForRecovery, Collection<Drive> drives) {
        try {
            List<Drive> physicallyConnectedDrives = getDrivesConnectedWithEndpoint(drives);
            physicallyConnectedDrives.forEach(drive -> attachDriveToComposedNode(composedNodesEligibleForRecovery, drive));

        } catch (IllegalStateException e) {
            logger.e("Error occurred when reattaching drives: " + e.getMessage(), e);
        }
    }

    private void attachDriveToComposedNode(List<ComposedNode> composedNodesEligibleForRecovery, Drive drive) {
        Map<String, ComputerSystem> localPcieConnectionIds = getComputerSystemPcieConnectionIdMap(composedNodesEligibleForRecovery);

        pciePortDao.getUpstreamPortsByDiscoverableEntity(drive)
            .stream()
            .map(Port::getPcieConnectionIds)
            .flatMap(Collection::stream)
            .filter(localPcieConnectionIds::containsKey)
            .map(localPcieConnectionIds::get)
            .filter(computerSystem -> computerSystem.getComposedNode() != null)
            .map(ComputerSystem::getComposedNode)
            .filter(composedNodesEligibleForRecovery::contains)
            .peek(node -> logger.t("Reattaching drive({}) to the node({})", drive, node))
            .forEach(node -> attachEndpoint(node, drive));
    }

    private List<Drive> getDrivesConnectedWithEndpoint(Collection<Drive> drives) {
        return drives.stream()
            .filter(drive -> drive.getConnectedEntity() != null)
            .filter(drive -> drive.getConnectedEntity().getEndpoint() != null)
            .collect(toList());
    }

    private Map<String, ComputerSystem> getComputerSystemPcieConnectionIdMap(List<ComposedNode> composedNodesEligibleForRecovery) {
        Set<ComputerSystem> computerSystems = composedNodesEligibleForRecovery.stream()
            .filter(node -> node.getComputerSystem() != null)
            .map(ComposedNode::getComputerSystem)
            .collect(toSet());

        Map<String, ComputerSystem> pcieConnectionIdToComputerSystemMap = new HashMap<>();

        for (ComputerSystem computerSystem : computerSystems) {

            for (String cableId : computerSystem.getPcieConnectionId()) {
                pcieConnectionIdToComputerSystemMap.put(cableId, computerSystem);
            }
        }

        return pcieConnectionIdToComputerSystemMap;
    }

    private void attachEndpoint(ComposedNode node, Drive drive) {
        node.addDrive(drive);
        drive.getMetadata().setAllocated(true);
    }

    private void recoverComposedNodes(Collection<ComposedNode> composedNodesEligibleForRecovery) {
        composedNodesEligibleForRecovery.stream()
            .filter(this::fulfillsComputerSystemRequirements)
            .filter(this::fulfillsRemoteTargetRequirements)
            .filter(this::fulfillsDriveRequirements)
            .forEach(this::recoverComposedNode);
    }

    private void recoverComposedNode(ComposedNode composedNode) {
        ComposedNodeState composedNodeState = ASSEMBLED;
        Status composedNodeStatus = new Status(ENABLED, OK, OK);

        composedNode.setEligibleForRecovery(false);
        composedNode.setStatus(composedNodeStatus);

        composedNode.setComposedNodeState(composedNodeState);
        logger.i("Putting composed Node: {} into {} state and {} status",
            composedNode.getId(),
            composedNodeState,
            composedNodeStatus);
    }

    private void reattachRemoteTargets(Collection<ComposedNode> nodesEligibleForRecovery, Collection<RemoteTarget> targets) {
        for (ComposedNode composedNode : nodesEligibleForRecovery) {
            String associatedRemoteTargetIqn = composedNode.getAssociatedRemoteTargetIqn();
            UUID associatedStorageServiceUuid = composedNode.getAssociatedStorageServiceUuid();

            try {
                RemoteTarget associatedRemoteTarget = singleOrNull(targets.stream()
                    .filter(target -> target.getService().getUuid() != null)
                    .filter(DiscoverableEntity::isEnabledAndHealthy)
                    .filter(target -> Objects.equals(target.getService().getUuid(), associatedStorageServiceUuid))
                    .filter(target -> target.getTargetIqn() != null)
                    .filter(target -> Objects.equals(target.getTargetIqn(), associatedRemoteTargetIqn))
                    .collect(toList()));

                if (associatedRemoteTarget != null) {
                    associatedRemoteTarget.getMetadata().setAllocated(true);
                    composedNode.addRemoteTarget(associatedRemoteTarget);
                }
            } catch (IllegalStateException e) {
                logger.e("There are more than one remote target with IQN: {} in service with UUID: {}",
                    associatedRemoteTargetIqn,
                    associatedStorageServiceUuid);
            }
        }
    }

    private void reattachComputerSystems(Collection<ComposedNode> nodesEligibleForRecovery,
                                         Collection<ComputerSystem> computerSystems) {
        for (ComposedNode composedNode : nodesEligibleForRecovery) {
            UUID associatedComputerSystemUuid = composedNode.getAssociatedComputerSystemUuid();
            UUID associatedComputeServiceUuid = composedNode.getAssociatedComputeServiceUuid();

            try {
                ComputerSystem associatedComputerSystem = singleOrNull(computerSystems.stream()
                    .filter(computerSystem -> computerSystem.getService().getUuid() != null)
                    .filter(DiscoverableEntity::isEnabledAndHealthy)
                    .filter(computerSystem -> Objects.equals(computerSystem.getService().getUuid(), associatedComputeServiceUuid))
                    .filter(computerSystem -> computerSystem.getUuid() != null)
                    .filter(computerSystem -> Objects.equals(computerSystem.getUuid(), associatedComputerSystemUuid))
                    .collect(toList()));

                if (associatedComputerSystem != null) {
                    associatedComputerSystem.getMetadata().setAllocated(true);
                    composedNode.setComputerSystem(associatedComputerSystem);
                }
            } catch (IllegalStateException e) {
                logger.e("There are more than one computer system with UUID: {} in service with UUID: {}",
                    associatedComputerSystemUuid,
                    associatedComputeServiceUuid);
            }
        }
    }

    private List<DiscoverableEntity> getEnabledAndHealthyDiscoverableEntities(Set<Entity> entities) {
        return entities.stream()
            .filter(entity -> DiscoverableEntity.class.isAssignableFrom(entity.getClass()))
            .map(DiscoverableEntity.class::cast)
            .filter(DiscoverableEntity::canBeAllocated)
            .collect(toList());
    }

    private <T extends DiscoverableEntity> List<T> getNotEnabledAssetsOfType(Set<Entity> entities, Class<T> clazz) {
        return entities.stream()
            .filter(entity -> clazz.isAssignableFrom(entity.getClass()))
            .map(clazz::cast)
            .filter(this::isAssetNotEnabled)
            .peek(asset -> logger.t("Not enabled asset: " + asset))
            .collect(toList());
    }

    @Transactional(MANDATORY)
    public void disableComposedNode(ComposedNode composedNode) {
        if (composedNode != null) {
            if (composedNode.isInAnyOfStates(ASSEMBLED)) {
                composedNode.setEligibleForRecovery(true);
            }

            composedNode.setComposedNodeState(FAILED);
            composedNode.setStatus(OFFLINE_CRITICAL_STATUS);
            logger.i("Putting composed Node: {} into {} state and {} status",
                composedNode.getId(),
                FAILED,
                OFFLINE_CRITICAL_STATUS
            );
        }
    }

    private boolean isAssetNotEnabled(DiscoverableEntity discoverableEntity) {
        return discoverableEntity != null
            && (discoverableEntity.getStatus() == null
            || !of(ENABLED, STANDBY_OFFLINE).anyMatch(state -> state == discoverableEntity.getStatus().getState()))
            || of(CRITICAL).anyMatch(state -> state == discoverableEntity.getStatus().getHealth());
    }

    private boolean fulfillsComputerSystemRequirements(ComposedNode composedNode) {
        return composedNode.getComputerSystem() != null ? composedNode.getComputerSystem().isEnabledAndHealthy() : false;
    }

    private boolean fulfillsRemoteTargetRequirements(ComposedNode composedNode) {
        boolean composedNodeWithRemoteTarget = composedNode.getAssociatedStorageServiceUuid() != null
            && composedNode.getAssociatedRemoteTargetIqn() != null;

        if (composedNodeWithRemoteTarget) {
            if (composedNode.getRemoteTargets().size() > 0) {
                return composedNode.getRemoteTargets().stream().noneMatch(target -> !target.isEnabledAndHealthy());
            } else {
                return false;
            }
        } else {
            return true;
        }
    }

    private boolean fulfillsDriveRequirements(ComposedNode composedNode) {
        return composedNode.getDrives().size() == composedNode.getNumberOfRequestedDrives();
    }
}
