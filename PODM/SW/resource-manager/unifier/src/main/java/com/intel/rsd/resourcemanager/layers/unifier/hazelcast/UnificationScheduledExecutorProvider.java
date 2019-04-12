/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import com.hazelcast.config.ScheduledExecutorConfig;
import lombok.val;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class UnificationScheduledExecutorProvider {
    public static final String TASK_SCHEDULER_NAME = "unification-tasks-scheduler";

    @Bean
    public ScheduledExecutorConfig scheduledExecutorConfig(UnificationTaskConfig unificationTaskConfig) {
        val unificationTaskScheduledExecutorConfig = new ScheduledExecutorConfig();
        unificationTaskScheduledExecutorConfig.setPoolSize(unificationTaskConfig.getPoolSize());
        unificationTaskScheduledExecutorConfig.setName(TASK_SCHEDULER_NAME);
        return unificationTaskScheduledExecutorConfig;
    }
}
