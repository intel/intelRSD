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

package com.intel.podm.discovery.external;

import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.config.base.dto.EventsConfig;
import com.intel.podm.discovery.external.event.EventSubscribeRunner;
import com.intel.podm.discovery.external.event.EventSubscriptionTask;
import com.intel.podm.discovery.external.event.EventSubscriptionTaskFactory;

import javax.annotation.Resource;
import javax.ejb.Singleton;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.inject.Inject;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ScheduledFuture;

import static java.util.concurrent.TimeUnit.SECONDS;

@Singleton
public class DiscoveryScheduler {

    @Resource(name = "default")
    private ManagedScheduledExecutorService discoveryManagedExecutorService;

    @Resource(lookup = "java:/EventSubscribeExecutor")
    private ManagedScheduledExecutorService eventManagedExecutorService;

    @Inject
    private DiscoveryRunner discoveryRunner;

    @Inject
    private EventSubscribeRunner eventSubscribeRunner;

    @Inject
    private EventSubscriptionTaskFactory eventSubscriptionTaskFactory;

    @Inject @Config
    private Holder<DiscoveryConfig> discoveryConfig;

    @Inject @Config
    private Holder<EventsConfig> eventsConfig;

    private Map<UUID, ScheduledFuture> discoveryTasks = new HashMap<>();

    private Map<UUID, ScheduledFuture> eventSubscriptionTasks = new HashMap<>();
    private Set<UUID> servicesWithScheduledDiscovery = ConcurrentHashMap.newKeySet();


    public void scheduleServiceDiscoveryWithEventServiceSubscription(UUID serviceUuid) {
        long discoveryIntervalSeconds = discoveryConfig.get().getDiscoveryIntervalSeconds();
        long discoveryDelaySeconds = 0;

        if (!eventSubscriptionTasks.containsKey(serviceUuid)) {
            triggerDiscoveryAndScheduleEventService(serviceUuid);
            //discovery was made during scheduling event service, so first scheduled discovery can be delayed
            discoveryDelaySeconds = discoveryIntervalSeconds;
        }

        if (!discoveryTasks.containsKey(serviceUuid)) {
            ScheduledFuture<?> discoveryTask = discoveryManagedExecutorService.scheduleWithFixedDelay(
                    () -> triggerDiscovery(serviceUuid), discoveryDelaySeconds, discoveryIntervalSeconds, SECONDS);
            discoveryTasks.put(serviceUuid, discoveryTask);
        }
    }

    private void triggerDiscoveryAndScheduleEventService(UUID serviceUuid) {
        //run synchronously for first time
        triggerDiscovery(serviceUuid);
        subscribeAndScheduleEventService(serviceUuid);
    }

    public void subscribeAndScheduleEventService(UUID serviceUuid) {
        eventSubscribeRunner.subscribeIfNotAlreadySubscribed(serviceUuid);
        long eventSubscriptionIntervalSeconds = eventsConfig.get().getEventSubscriptionIntervalSeconds();
        EventSubscriptionTask task = eventSubscriptionTaskFactory.create(serviceUuid);
        ScheduledFuture<?> eventSubscriptionTask =
                eventManagedExecutorService.scheduleWithFixedDelay(
                        task,
                        eventSubscriptionIntervalSeconds,
                        eventSubscriptionIntervalSeconds,
                        SECONDS
                );
        eventSubscriptionTasks.put(serviceUuid, eventSubscriptionTask);
    }

    public void cancel(UUID serviceUuid) {
        cancelTaskForService(discoveryTasks, serviceUuid);
        cancelTaskForService(eventSubscriptionTasks, serviceUuid);
    }

    private void cancelTaskForService(Map<UUID, ScheduledFuture> tasks, UUID serviceUuid) {
        if (tasks.containsKey(serviceUuid)) {
            tasks.get(serviceUuid).cancel(false);
            tasks.remove(serviceUuid);
        }
    }

    public void triggerDiscovery(UUID serviceUuid) {
        if (servicesWithScheduledDiscovery.add(serviceUuid)) {
            discoveryManagedExecutorService.schedule(() -> {
                servicesWithScheduledDiscovery.remove(serviceUuid);
                discoveryRunner.run(serviceUuid);
            }, discoveryConfig.get().getTriggeredDiscoveryDelaySeconds(), SECONDS);
        }
    }
}
