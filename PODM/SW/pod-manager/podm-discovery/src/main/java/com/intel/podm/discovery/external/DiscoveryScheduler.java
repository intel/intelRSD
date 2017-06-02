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

package com.intel.podm.discovery.external;

import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.synchronization.TaskCoordinator;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.config.base.dto.EventsConfig;
import com.intel.podm.discovery.external.deep.DeepDiscoveryTriggeringTask;
import com.intel.podm.discovery.external.event.EventSubscribeRunner;
import com.intel.podm.discovery.external.event.EventSubscriptionTaskFactory;

import javax.ejb.Singleton;
import javax.inject.Inject;
import javax.inject.Named;
import java.util.HashMap;
import java.util.Map;
import java.util.UUID;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;

import static java.util.concurrent.TimeUnit.SECONDS;

@Singleton
public class DiscoveryScheduler {
    private static final int DEEP_DISCOVERY_INTERVAL = 10;

    @Inject
    @Named("TasksExecutor")
    private ScheduledExecutorService discoveryTaskExecutor;

    @Inject
    @Named("EventsExecutor")
    private ScheduledExecutorService eventSubscriptionTaskExecutor;

    @Inject
    private EventSubscribeRunner eventSubscribeRunner;

    @Inject
    private EventSubscriptionTaskFactory eventSubscriptionTaskFactory;

    @Inject
    @Config
    private Holder<DiscoveryConfig> discoveryConfig;

    @Inject
    private TaskCoordinator taskCoordinator;

    @Inject
    private BeanFactory beanFactory;

    @Inject
    @Config
    private Holder<EventsConfig> eventsConfig;

    //TODO: consider refactor and move below hashmaps to another class
    private Map<UUID, ScheduledFuture> discoveryTasks = new HashMap<>();

    private Map<UUID, ScheduledFuture> eventSubscriptionTasks = new HashMap<>();

    private Map<UUID, ScheduledFuture> deepDiscoveryTasks = new HashMap<>();

    private Map<UUID, DiscoveryRunner> discoveryRunners = new HashMap<>();

    public void scheduleServiceDiscoveryWithEventServiceSubscription(UUID serviceUuid) {
        DiscoveryConfig discoveryConfig = this.discoveryConfig.get();
        long discoveryIntervalSeconds = discoveryConfig.getDiscoveryIntervalSeconds();
        long discoveryDelaySeconds = 0;

        boolean scheduledSubscriptionTask = scheduleEventServiceSubscription(serviceUuid);
        discoveryDelaySeconds = scheduledSubscriptionTask ? discoveryIntervalSeconds : discoveryDelaySeconds;

        scheduleDiscovery(serviceUuid, discoveryIntervalSeconds, discoveryDelaySeconds);

        if (discoveryConfig.isDeepDiscoveryEnabled()) {
            scheduleDeepDiscoveryTriggeringTask(serviceUuid);
        }
    }

    private void scheduleDeepDiscoveryTriggeringTask(UUID serviceUuid) {
        if (!deepDiscoveryTasks.containsKey(serviceUuid)) {
            ScheduledFuture<?> deepDiscoveryTriggeringTask = discoveryTaskExecutor.scheduleWithFixedDelay(
                () -> enqueueDeepDiscoveryTriggeringTask(serviceUuid),
                DEEP_DISCOVERY_INTERVAL,
                DEEP_DISCOVERY_INTERVAL,
                SECONDS
            );
            deepDiscoveryTasks.put(serviceUuid, deepDiscoveryTriggeringTask);
        }
    }

    private void scheduleDiscovery(UUID serviceUuid, long discoveryIntervalSeconds, long discoveryDelaySeconds) {
        if (!discoveryTasks.containsKey(serviceUuid)) {
            ScheduledFuture<?> discoveryTask = discoveryTaskExecutor.scheduleWithFixedDelay(
                () -> enqueueDiscovery(serviceUuid),
                discoveryDelaySeconds,
                discoveryIntervalSeconds,
                SECONDS
            );
            discoveryTasks.put(serviceUuid, discoveryTask);
        } else {
            enqueueDiscovery(serviceUuid);
        }
    }

    private boolean scheduleEventServiceSubscription(UUID serviceUuid) {
        if (!eventSubscriptionTasks.containsKey(serviceUuid)) {
            triggerDiscoveryAndScheduleEventService(serviceUuid);
            //discovery was made during scheduling event service, so first scheduled discovery can be delayed
            return true;
        }
        return false;
    }

    private void triggerDiscoveryAndScheduleEventService(UUID serviceUuid) {
        //run synchronously for first time
        enqueueDiscovery(serviceUuid);
        subscribeAndScheduleEventService(serviceUuid);
    }

    private void subscribeAndScheduleEventService(UUID serviceUuid) {
        eventSubscribeRunner.subscribeIfNotAlreadySubscribed(serviceUuid);
        long eventSubscriptionIntervalSeconds = eventsConfig.get().getEventSubscriptionIntervalSeconds();
        ScheduledFuture<?> eventSubscriptionTask =
            eventSubscriptionTaskExecutor.scheduleWithFixedDelay(
                eventSubscriptionTaskFactory.create(serviceUuid),
                eventSubscriptionIntervalSeconds,
                eventSubscriptionIntervalSeconds,
                SECONDS
            );
        eventSubscriptionTasks.put(serviceUuid, eventSubscriptionTask);
    }

    public void cancel(UUID serviceUuid) {
        cancelTaskForService(discoveryTasks, serviceUuid);
        cancelTaskForService(eventSubscriptionTasks, serviceUuid);
        cancelTaskForService(deepDiscoveryTasks, serviceUuid);
        discoveryRunners.remove(serviceUuid);
    }

    private void cancelTaskForService(Map<UUID, ScheduledFuture> tasks, UUID serviceUuid) {
        if (tasks.containsKey(serviceUuid)) {
            tasks.get(serviceUuid).cancel(false);
            tasks.remove(serviceUuid);
        }
    }

    public void enqueueDiscovery(UUID serviceUuid) {
        DiscoveryRunner discoveryRunner = getDiscoveryRunner(serviceUuid);
        taskCoordinator.registerAsync(serviceUuid, discoveryRunner);
    }

    private DiscoveryRunner getDiscoveryRunner(UUID serviceUuid) {
        DiscoveryRunner discoveryRunner = discoveryRunners.get(serviceUuid);
        if (discoveryRunner == null) {
            discoveryRunner = beanFactory.create(DiscoveryRunner.class);
            discoveryRunner.setServiceUuid(serviceUuid);
            discoveryRunners.put(serviceUuid, discoveryRunner);
        }

        return discoveryRunner;
    }

    private void enqueueDeepDiscoveryTriggeringTask(UUID serviceUuid) {
        DeepDiscoveryTriggeringTask deepDiscoveryTriggeringTask = beanFactory.create(DeepDiscoveryTriggeringTask.class);
        deepDiscoveryTriggeringTask.setServiceUuid(serviceUuid);
        taskCoordinator.registerAsync(serviceUuid, deepDiscoveryTriggeringTask);
    }
}
