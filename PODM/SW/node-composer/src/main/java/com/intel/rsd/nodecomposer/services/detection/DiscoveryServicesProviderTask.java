/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.services.detection;

import com.intel.rsd.nodecomposer.services.configuration.ConfigurationTasksManager;
import com.intel.rsd.services.common.hazelcast.ScheduledWithFixedDelayOnLeaderTask;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.time.Duration;

import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.DEFAULT_TASK_SCHEDULER;

@Component
public class DiscoveryServicesProviderTask extends ScheduledWithFixedDelayOnLeaderTask {
    private final Duration scanInterval;
    private final DiscoveryServicesProvider discoveryServicesProvider;
    private final ConfigurationTasksManager configurationTasksManager;

    @Autowired
    public DiscoveryServicesProviderTask(@Qualifier(DEFAULT_TASK_SCHEDULER) TaskScheduler taskScheduler,
                                         @Value("${detection.scan-interval-seconds:10}") long scanIntervalSeconds,
                                         DiscoveryServicesProvider discoveryServicesProvider,
                                         ConfigurationTasksManager configurationTasksManager) {
        super(taskScheduler);
        this.scanInterval = Duration.ofSeconds(scanIntervalSeconds);
        this.discoveryServicesProvider = discoveryServicesProvider;
        this.configurationTasksManager = configurationTasksManager;
    }

    @Override
    public void run() {
        configurationTasksManager.scheduleConfigurationTasks(discoveryServicesProvider.provide());
    }

    @Override
    protected Duration getFixedDelay() {
        return scanInterval;
    }

    @Override
    public String toString() {
        return "services-provider-task";
    }
}
