/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.concurrent;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.scheduling.concurrent.ThreadPoolTaskScheduler;

import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.DEFAULT_TASK_SCHEDULER;
import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.DISCOVERY_TASK_SCHEDULER;

@Configuration
public class TaskSchedulers {
    private static final int DEFAULT_TASK_SCHEDULER_POOL_SIZE = 2;
    private static final int DISCOVERY_TASK_SCHEDULER_POOL_SIZE = 8;

    @Bean(name = DEFAULT_TASK_SCHEDULER)
    public TaskScheduler getDefaultTaskScheduler() {
        ThreadPoolTaskScheduler defaultTaskScheduler = new ThreadPoolTaskScheduler();
        defaultTaskScheduler.setPoolSize(DEFAULT_TASK_SCHEDULER_POOL_SIZE);
        return defaultTaskScheduler;
    }

    @Bean(name = DISCOVERY_TASK_SCHEDULER)
    public TaskScheduler getDiscoveryTaskScheduler() {
        ThreadPoolTaskScheduler discoveryTaskScheduler = new ThreadPoolTaskScheduler();
        discoveryTaskScheduler.setPoolSize(DISCOVERY_TASK_SCHEDULER_POOL_SIZE);
        return discoveryTaskScheduler;
    }
}
