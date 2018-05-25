/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.common.types.Id;

import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.inject.Named;
import javax.transaction.Transactional;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.SYNCHRONIZED_TASK_EXECUTOR;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.READ;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static javax.transaction.Transactional.TxType.SUPPORTS;

@Singleton
public class DeepDiscoveryImpl implements DeepDiscovery {
    private static final int DEEP_DISCOVERY_INTERVAL = 10;

    @Inject
    @Named(SYNCHRONIZED_TASK_EXECUTOR)
    private ScheduledExecutorService discoveryTaskExecutor;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private BeanFactory beanFactory;

    @Inject
    private DeepDiscoveryFinalizer deepDiscoveryFinalizer;

    @Inject
    private DeepDiscoveryHelper deepDiscoveryHelper;

    private Map<UUID, ScheduledFuture> deepDiscoveryTasks = new HashMap<>();

    /**
     * Delegates triggering deep discovery to DeepDiscoverHelper
     * that operates each time within a new transaction.
     * <p/>
     * Using LockType.READ will enable triggering deep discovery
     * process for multiple computerSystemId's as long as there
     * are no locks held either due to:
     * - finalizing discovery or
     * - setting up / turning off scheduled deep discovery task for external services.
     *
     * @see DeepDiscoveryHelper
     */
    @Override
    @Lock(READ)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void triggerForComputerSystem(Id computerSystemId) throws DeepDiscoveryException {
        deepDiscoveryHelper.triggerDeepDiscovery(computerSystemId);
    }

    /**
     * Delegates finalizing deep discovery of computer system to DeepDiscoveryFinalizer.
     * LockType.WRITE is used to ensure that simultaneous triggerForComputerSystem
     * for the same computerSystemId won't occur.
     * <p/>
     * Handling transactions (repeating in particular) should be handled by caller.
     *
     * @see DeepDiscoveryFinalizer
     */
    @Override
    @Lock(WRITE)
    @Transactional(MANDATORY)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void finalizeForComputerSystem(Id computerSystemId) {
        deepDiscoveryFinalizer.finalizeSuccessfulDeepDiscovery(computerSystemId);
    }

    /**
     * LockType.WRITE used due to concurrent access to deep discovery tasks map that modifies it (put operation)
     */
    @Override
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void triggerForExternalService(UUID externalServiceUuid) {
        if (!deepDiscoveryTasks.containsKey(externalServiceUuid)) {
            ScheduledFuture<?> deepDiscoveryTriggeringTask = discoveryTaskExecutor.scheduleWithFixedDelay(
                () -> enqueueDeepDiscoveryTriggeringTask(externalServiceUuid),
                DEEP_DISCOVERY_INTERVAL,
                DEEP_DISCOVERY_INTERVAL,
                SECONDS
            );
            deepDiscoveryTasks.put(externalServiceUuid, deepDiscoveryTriggeringTask);
        }
    }

    /**
     * LockType.WRITE used due to concurrent access to deep discovery tasks map that modifies it (remove operation)
     */
    @Override
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void cancelForExternalService(UUID externalServiceUuid) {
        if (deepDiscoveryTasks.containsKey(externalServiceUuid)) {
            deepDiscoveryTasks.get(externalServiceUuid).cancel(false);
            deepDiscoveryTasks.remove(externalServiceUuid);
        }
    }

    private void enqueueDeepDiscoveryTriggeringTask(UUID serviceUuid) {
        DeepDiscoveryTriggeringTask deepDiscoveryTriggeringTask = beanFactory.create(DeepDiscoveryTriggeringTask.class);
        deepDiscoveryTriggeringTask.setServiceUuid(serviceUuid);
        taskCoordinator.registerAsync(serviceUuid, deepDiscoveryTriggeringTask);
    }
}
