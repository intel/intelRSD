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

package com.intel.podm.discovery.external.deep;

import com.intel.podm.common.logger.Logger;

import javax.annotation.Resource;
import javax.ejb.Singleton;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.inject.Inject;
import java.util.concurrent.RejectedExecutionException;

import static java.util.concurrent.TimeUnit.SECONDS;

@Singleton
public class DeepDiscoveryTaskScheduler {

    public static final long DEEP_DISCOVERY_INTERVAL = 10L;

    @Resource(lookup = "java:/DeepDiscoveryExecutor")
    private ManagedScheduledExecutorService executor;

    @Inject
    private DeepDiscoveryTriggeringTask deepDiscoveryTriggeringTask;

    @Inject
    private Logger logger;

    private boolean isScheduled;

    public void start() {

        if (isScheduled) {
            throw new UnsupportedOperationException("DeepDiscoveryScheduler already started");
        }

        try {
            executor.scheduleWithFixedDelay(deepDiscoveryTriggeringTask, 0L, DEEP_DISCOVERY_INTERVAL, SECONDS);
            isScheduled = true;
        } catch (RejectedExecutionException e) {
            logger.e("Application failed to start properly. DeepDiscoveryScheduler is disabled.", e);
            throw e;
        }
    }
}
