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

package com.intel.rsd.resourcemanager.runner.events;

import com.hazelcast.core.HazelcastInstance;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration;
import com.intel.rsd.resourcemanager.runner.requiredlayer.RequiredServiceFinder;
import com.intel.rsd.services.common.hazelcast.DistributedTaskSchedulerTask;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.ApplicationContext;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.time.Duration;
import java.util.Set;
import java.util.stream.Collectors;

@Component
@Slf4j
public class EventSubscriptionTaskSchedulerTask extends DistributedTaskSchedulerTask {

    private final EventsConfiguration eventsConfiguration;

    private final RequiredServiceFinder requiredServiceFinder;

    public EventSubscriptionTaskSchedulerTask(ApplicationContext applicationContext,
                                              HazelcastInstance hazelcastInstance,
                                              TaskScheduler taskScheduler,
                                              EventsConfiguration eventsConfiguration,
                                              RequiredServiceFinder requiredServiceFinder) {

        super(taskScheduler, hazelcastInstance.getScheduledExecutorService("event-subscriber"), applicationContext);
        this.requiredServiceFinder = requiredServiceFinder;
        this.eventsConfiguration = eventsConfiguration;
    }

    @Override
    protected String getTypeNameOfDistributedTasks() {
        return "EventSubscriptionTask";
    }

    @Override
    public String toString() {
        return "Monitor of Event Subscription Tasks";
    }

    @Override
    protected Set<String> getIdsOfTasksThatShouldBeRunning() {
        return requiredServiceFinder.getServices().stream().map(ServiceId::getValue).collect(Collectors.toSet());
    }

    @Override
    protected Runnable createTask(String taskId) {
        return new EventSubscriptionTask(new ServiceId(taskId));
    }

    @Override
    protected Duration getDistributedTaskDelay() {
        return eventsConfiguration.getReceiver().getSubscriptionTask().getTaskDelay();
    }

    @Override
    protected Duration getFixedDelay() {
        return eventsConfiguration.getReceiver().getSubscriptionTask().getTaskSchedulerDelay();
    }

}
