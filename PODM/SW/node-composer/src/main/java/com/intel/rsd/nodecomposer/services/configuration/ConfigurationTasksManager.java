/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.configuration;

import com.intel.rsd.nodecomposer.types.discovery.DiscoveryServiceEndpoint;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import com.intel.rsd.nodecomposer.utils.lock.Lock;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.event.EventListener;
import org.springframework.integration.leader.event.OnRevokedEvent;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.UUID;
import java.util.concurrent.ScheduledFuture;

import static com.google.common.collect.Sets.difference;
import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.DISCOVERY_TASK_SCHEDULER;
import static org.apache.commons.lang3.time.DateUtils.MILLIS_PER_SECOND;

@Slf4j
@Component
public class ConfigurationTasksManager {
    final Map<UUID, ScheduledFuture<?>> configurationTasks = new HashMap<>();

    private final long configurerIntervalSeconds;
    private final TaskScheduler configurationTaskScheduler;
    private final NodeComposerBeanFactory beanFactory;

    @Autowired
    public ConfigurationTasksManager(@Value("${discovery-service.configurer-interval-seconds:300}") long configurerIntervalSeconds,
                                     @Qualifier(DISCOVERY_TASK_SCHEDULER) TaskScheduler configurationTaskScheduler,
                                     NodeComposerBeanFactory beanFactory) {
        this.configurerIntervalSeconds = configurerIntervalSeconds;
        this.configurationTaskScheduler = configurationTaskScheduler;
        this.beanFactory = beanFactory;
    }

    /**
     * LockType.WRITE used due to concurrent access to configuration tasks map that modifies it (put operation)
     */
    @Lock
    public void scheduleConfigurationTasks(Map<UUID, DiscoveryServiceEndpoint> discoveryServiceEndpoints) {
        Set<UUID> toBeScheduled = discoveryServiceEndpoints.keySet();
        Set<UUID> alreadyScheduled = configurationTasks.keySet();

        difference(alreadyScheduled, toBeScheduled).immutableCopy().forEach(uuid -> {
            ScheduledFuture<?> configurationTask = configurationTasks.remove(uuid);
            if (configurationTask != null) {
                log.debug("Configuration task cancelled for DiscoveryService: {}", uuid);
                configurationTask.cancel(false);
            }
        });

        difference(toBeScheduled, alreadyScheduled).immutableCopy().forEach(uuid -> {
            DiscoveryServiceEndpoint discoveryServiceEndpoint = discoveryServiceEndpoints.get(uuid);
            ScheduledFuture<?> configurationTask = scheduleConfigurationTask(discoveryServiceEndpoint);
            configurationTasks.putIfAbsent(uuid, configurationTask);
        });
    }

    @EventListener(OnRevokedEvent.class)
    void onRevoked() {
        configurationTasks.values().forEach(scheduledFuture -> scheduledFuture.cancel(true));
    }

    private ScheduledFuture<?> scheduleConfigurationTask(DiscoveryServiceEndpoint discoveryServiceEndpoint) {
        long configurerDelaySeconds = 0;
        log.info("Scheduling configuration task for DiscoveryService {} that will execute every {}s", discoveryServiceEndpoint, configurerIntervalSeconds);

        return configurationTaskScheduler.scheduleWithFixedDelay(
            createDiscoveryServiceConfigurationTask(discoveryServiceEndpoint),
            new Date(System.currentTimeMillis() + configurerDelaySeconds * MILLIS_PER_SECOND),
            configurerIntervalSeconds * MILLIS_PER_SECOND
        );
    }

    private DiscoveryServiceConfigurationTask createDiscoveryServiceConfigurationTask(DiscoveryServiceEndpoint discoveryServiceEndpoint) {
        DiscoveryServiceConfigurationTask task = beanFactory.create(DiscoveryServiceConfigurationTask.class);
        task.setDiscoveryServiceEndpoint(discoveryServiceEndpoint);
        return task;
    }
}

