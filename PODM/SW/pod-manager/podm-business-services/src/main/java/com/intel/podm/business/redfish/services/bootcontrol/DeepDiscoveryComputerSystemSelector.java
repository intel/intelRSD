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

package com.intel.podm.business.redfish.services.bootcontrol;

import com.intel.podm.business.entities.dao.ChassisDao;
import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.UUID;
import java.util.function.Predicate;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.types.DeepDiscoveryState.INITIAL;
import static com.intel.podm.common.types.DeepDiscoveryState.SCHEDULED_MANUALLY;
import static com.intel.podm.common.types.DeepDiscoveryState.WAITING_TO_START;
import static com.intel.podm.common.types.DiscoveryState.DEEP_IN_PROGRESS;
import static com.intel.podm.common.types.SystemType.PHYSICAL;
import static java.lang.Math.max;
import static java.lang.String.format;
import static java.util.Comparator.comparing;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

/**
 * Class responsible for selecting and marking computer systems for deep discovery process.
 */
@Dependent
class DeepDiscoveryComputerSystemSelector {
    @Inject
    private Logger logger;

    @Inject
    @Config(refreshable = true)
    private Holder<DiscoveryConfig> config;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private ChassisDao chassisDao;

    @Transactional(REQUIRES_NEW)
    public void markComputerSystemsForDeepDiscovery(UUID serviceUuid) {
        ExternalService externalService = externalServiceDao.tryGetUniqueExternalServiceByUuid(serviceUuid);
        if (externalService == null) {
            throw new RuntimeException(
                format("Marking Computer Systems for Deep Discovery failed, unique Service with UUID: %s was not found.", serviceUuid)
            );
        }

        List<ComputerSystem> computerSystems = getPhysicalComputerSystems(externalService);
        Predicate<ComputerSystem> computerSystemIsContainedByDrawer = computerSystem -> computerSystem.getDrawerChassis().isPresent();

        Set<ComputerSystem> computerSystemsWithDrawer = filterComputerSystems(computerSystems, computerSystemIsContainedByDrawer);
        Map<Chassis, DrawerDeepDiscoveryCounter> drawerDeepDiscoveryCounterMap = buildDrawerDeepDiscoveryCounterMap(computerSystemsWithDrawer);
        drawerDeepDiscoveryCounterMap.forEach(this::processDrawer);

        Set<ComputerSystem> computerSystemsWithoutDrawer = filterComputerSystems(computerSystems, computerSystemIsContainedByDrawer.negate());
        putAuthorizedComputerSystemsToWaitingStateWithLimit(computerSystemsWithoutDrawer, computerSystemsWithoutDrawer.size());
    }

    private List<ComputerSystem> getPhysicalComputerSystems(ExternalService externalService) {
        return externalService.getOwned(ComputerSystem.class).stream()
            .filter(cs -> PHYSICAL.equals(cs.getSystemType()))
            .collect(toList());
    }

    private Set<ComputerSystem> filterComputerSystems(List<ComputerSystem> computerSystems, Predicate<ComputerSystem> predicate) {
        return computerSystems.stream()
            .filter(predicate)
            .collect(toSet());
    }

    private Map<Chassis, DrawerDeepDiscoveryCounter> buildDrawerDeepDiscoveryCounterMap(Set<ComputerSystem> computerSystems) {
        Map<Chassis, DrawerDeepDiscoveryCounter> drawerDeepDiscoveryCounterMap = new HashMap<>();

        for (ComputerSystem computerSystem : computerSystems) {
            Chassis drawerChassis = computerSystem.getDrawerChassis().get();
            drawerDeepDiscoveryCounterMap.putIfAbsent(drawerChassis, new DrawerDeepDiscoveryCounter());

            if (computerSystem.getMetadata().isBeingDeepDiscovered()) {
                drawerDeepDiscoveryCounterMap.get(drawerChassis).incrementComputerSystemsBeingDeepDiscoveredCount();
            } else if (qualifiesForDeepDiscovery(computerSystem)) {
                drawerDeepDiscoveryCounterMap.get(drawerChassis).incrementComputerSystemsWaitingForDeepDiscoveryCount();
            }
        }

        return drawerDeepDiscoveryCounterMap;
    }

    private void processDrawer(Chassis drawerChassis, DrawerDeepDiscoveryCounter drawerDeepDiscoveryCounter) {
        logStats(drawerChassis,
            drawerDeepDiscoveryCounter.getComputerSystemsBeingDeepDiscoveredCount(),
            drawerDeepDiscoveryCounter.getComputerSystemsWaitingForDeepDiscoveryCount());

        int maxComputerSystemsCountPerDrawerBeingDeepDiscovered = config.get().getMaxComputerSystemsCountPerDrawerBeingDeepDiscovered();
        int limit = max(maxComputerSystemsCountPerDrawerBeingDeepDiscovered - drawerDeepDiscoveryCounter.getComputerSystemsBeingDeepDiscoveredCount(), 0);

        Set<ComputerSystem> systems = drawerChassis.getAllComputerSystemsUnderneath();
        putAuthorizedComputerSystemsToWaitingStateWithLimit(systems, limit);
    }

    private void putAuthorizedComputerSystemsToWaitingStateWithLimit(Set<ComputerSystem> computerSystems, int limit) {
        computerSystems.stream()
            .filter(this::qualifiesForDeepDiscovery)
            .sorted(comparing(computerSystem -> computerSystem.getMetadata().isInAnyOfStates(SCHEDULED_MANUALLY)))
            .limit(limit)
            .forEach(this::putToWaitingState);
    }

    private void logStats(Chassis chassis, int computerSystemsBeingDeepDiscoveredCount, int toBeDeepDiscoveredCount) {
        if (computerSystemsBeingDeepDiscoveredCount + toBeDeepDiscoveredCount > 0) {
            logger.i("Drawer Chassis {} deep discovery: active ({}), waiting ({})", chassis.getId(),
                computerSystemsBeingDeepDiscoveredCount, toBeDeepDiscoveredCount);
        }
    }

    private boolean qualifiesForDeepDiscovery(ComputerSystem computerSystem) {
        return computerSystem.getMetadata().isInAnyOfStates(INITIAL, SCHEDULED_MANUALLY) && isUsable(computerSystem);
    }

    private boolean isUsable(ComputerSystem computerSystem) {
        return statusOf(computerSystem).isEnabled().isHealthy().verify()
            && computerSystem.getComposedNode() == null;
    }

    private void putToWaitingState(ComputerSystem computerSystem) {
        computerSystem.setDiscoveryState(DEEP_IN_PROGRESS);
        computerSystem.getMetadata().setDeepDiscoveryState(WAITING_TO_START);
        computerSystem.getMetadata().setAllocated(true);
    }

    private static final class DrawerDeepDiscoveryCounter {
        private int computerSystemsWaitingForDeepDiscoveryCount;
        private int computerSystemsBeingDeepDiscoveredCount;

        DrawerDeepDiscoveryCounter() {
            computerSystemsWaitingForDeepDiscoveryCount = 0;
            computerSystemsBeingDeepDiscoveredCount = 0;
        }

        public void incrementComputerSystemsWaitingForDeepDiscoveryCount() {
            computerSystemsWaitingForDeepDiscoveryCount++;
        }

        public void incrementComputerSystemsBeingDeepDiscoveredCount() {
            computerSystemsBeingDeepDiscoveredCount++;
        }

        public int getComputerSystemsWaitingForDeepDiscoveryCount() {
            return computerSystemsWaitingForDeepDiscoveryCount;
        }

        public int getComputerSystemsBeingDeepDiscoveredCount() {
            return computerSystemsBeingDeepDiscoveredCount;
        }
    }
}
