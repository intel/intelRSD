/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.synchronization.TaskCoordinator;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;
import java.time.Duration;

import static java.time.Duration.ZERO;

@Singleton
@DependsOn({"DiscoveryStartup"})
@Startup
public class ServiceRemovalTaskScheduler {
    public static final Duration TASK_DELAY = Duration.ofSeconds(10);
    @Inject
    private Logger logger;

    @Inject
    private ServiceRemovalTask task;

    @Inject
    private TaskCoordinator taskCoordinator;

    @PostConstruct
    public void schedule() {
        logger.d("Scheduling Service Removal Task...");
        taskCoordinator.scheduleWithFixedDelay("Service Removal Task", task, ZERO, TASK_DELAY);
    }
}
