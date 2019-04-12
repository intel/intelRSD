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

package com.intel.rsd.resourcemanager.layers.unifier.hazelcast;

import com.hazelcast.core.HazelcastInstance;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.unifier.UnificationLayer;
import com.intel.rsd.services.common.hazelcast.DistributedTaskSchedulerTask;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.ApplicationContext;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.time.Duration;
import java.util.Set;

import static com.intel.rsd.resourcemanager.layers.unifier.hazelcast.UnificationScheduledExecutorProvider.TASK_SCHEDULER_NAME;
import static java.time.Duration.ofSeconds;
import static java.util.stream.Collectors.toSet;

@Slf4j
@Component
class UnificationTaskSchedulerTask extends DistributedTaskSchedulerTask {
    private static final Duration DISTRIBUTED_TASK_SCHEDULING_DELAY = ofSeconds(10);

    private final UnificationLayer unificationLayer;
    private final Duration unificationTaskPeriodInSeconds;

    @Autowired
    UnificationTaskSchedulerTask(ApplicationContext applicationContext,
                                 HazelcastInstance hazelcastInstance,
                                 TaskScheduler taskScheduler,
                                 UnificationLayer unificationLayer,
                                 @Value("${unification_task_period_in_seconds:10}") int unificationTaskPeriodInSeconds) {

        super(taskScheduler, hazelcastInstance.getScheduledExecutorService(TASK_SCHEDULER_NAME), applicationContext);
        this.unificationLayer = unificationLayer;
        this.unificationTaskPeriodInSeconds = ofSeconds(unificationTaskPeriodInSeconds);
    }

    @Override
    public String toString() {
        return "Monitor of Unification Tasks";
    }

    @Override
    protected String getTypeNameOfDistributedTasks() {
        return "UnificationTask";
    }

    @Override
    protected Set<String> getIdsOfTasksThatShouldBeRunning() {
        return unificationLayer.getServices().stream()
            .map(ServiceId::getValue)
            .collect(toSet());
    }

    @Override
    protected Runnable createTask(String taskId) {
        return new UnificationTask(new ServiceId(taskId));
    }

    @Override
    protected Duration getFixedDelay() {
        return DISTRIBUTED_TASK_SCHEDULING_DELAY;
    }

    @Override
    protected Duration getDistributedTaskDelay() {
        return unificationTaskPeriodInSeconds;
    }
}


