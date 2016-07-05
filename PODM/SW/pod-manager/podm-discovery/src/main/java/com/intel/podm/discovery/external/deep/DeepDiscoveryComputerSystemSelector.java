/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.discovery.external.deep;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.INITIAL;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.SCHEDULED_MANUALLY;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.WAITING_TO_START;
import static com.intel.podm.common.types.DiscoveryState.DEEP_IN_PROGRESS;
import static java.lang.Math.max;
import static java.util.Comparator.comparing;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

/**
 * Class responsible for selecting and marking computer systems for deep discovery process.
 */
@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
public class DeepDiscoveryComputerSystemSelector {
    @Inject
    private Logger logger;

    @Inject
    @Config(refreshable = true)
    private Holder<DiscoveryConfig> config;

    @Inject
    private GenericDao genericDao;

    @Transactional(REQUIRES_NEW)
    @NumberOfRetriesOnRollback(3)
    public void markComputerSystemsForDeepDiscovery() {
        List<ComputerSystem> computerSystems = genericDao.findAll(ComputerSystem.class);
        Predicate<ComputerSystem> computerSystemIsContainedByDrawer = computerSystem -> computerSystem.getDrawerChassis().isPresent();

        Set<ComputerSystem> computerSystemsWithDrawer = filterComputerSystems(computerSystems, computerSystemIsContainedByDrawer);
        Map<Id, DrawerDeepDiscoveryCounter> drawerDeepDiscoveryCounterMap = buildDrawerDeepDiscoveryCounterMap(computerSystemsWithDrawer);
        drawerDeepDiscoveryCounterMap.entrySet().stream()
                .forEach(entry -> processDrawer(entry.getKey(), entry.getValue()));

        Set<ComputerSystem> computerSystemsWithoutDrawer = filterComputerSystems(computerSystems, computerSystemIsContainedByDrawer.negate());
        putAuthorizedComputerSystemsToWaitingStateWithLimit(computerSystemsWithoutDrawer, computerSystemsWithoutDrawer.size());
    }

    private Set<ComputerSystem> filterComputerSystems(List<ComputerSystem> computerSystems, Predicate<ComputerSystem> predicate) {
        return computerSystems.stream()
                .filter(predicate)
                .collect(toSet());
    }

    private Map<Id, DrawerDeepDiscoveryCounter> buildDrawerDeepDiscoveryCounterMap(Set<ComputerSystem> computerSystems) {
        Map<Id, DrawerDeepDiscoveryCounter> drawerDeepDiscoveryCounterMap = new HashMap<>();

        for (ComputerSystem computerSystem : computerSystems) {
            Id drawerId = computerSystem.getDrawerChassis().get().getId();
            drawerDeepDiscoveryCounterMap.putIfAbsent(drawerId, new DrawerDeepDiscoveryCounter());

            if (computerSystem.isBeingDeepDiscovered()) {
                drawerDeepDiscoveryCounterMap.get(drawerId).incrementComputerSystemsBeingDeepDiscoveredCount();
            } else if (qualifiesForDeepDiscovery(computerSystem)) {
                drawerDeepDiscoveryCounterMap.get(drawerId).incrementComputerSystemsWaitingForDeepDiscoveryCount();
            }
        }

        return drawerDeepDiscoveryCounterMap;
    }

    private void processDrawer(Id drawerId, DrawerDeepDiscoveryCounter drawerDeepDiscoveryCounter) {
        Chassis drawerChassis = genericDao.find(Chassis.class, drawerId);
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
                .sorted(comparing(computerSystem -> computerSystem.isInAnyOfStates(SCHEDULED_MANUALLY)))
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
        return computerSystem.isInAnyOfStates(INITIAL, SCHEDULED_MANUALLY) && isUsable(computerSystem);
    }

    private boolean isUsable(ComputerSystem computerSystem) {
        return !computerSystem.isStorageServiceHost()
                && computerSystem.isEnabledAndHealthy()
                && computerSystem.getComposedNode() == null;
    }

    private void putToWaitingState(ComputerSystem computerSystem) {
        computerSystem.setDiscoveryState(DEEP_IN_PROGRESS);
        computerSystem.setDeepDiscoveryState(WAITING_TO_START);
        computerSystem.setAllocated(true);
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
