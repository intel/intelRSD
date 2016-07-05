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

package com.intel.podm.services.detection;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.detection.tasks.ProvideEndpointsScheduledTask;
import com.intel.podm.services.detection.tasks.RecheckFailedUrisScheduledTask;

import javax.annotation.Resource;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.inject.Inject;
import java.util.concurrent.RejectedExecutionException;

import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;

/**
 * Class responsible for orchestrating service detection polling
 * and periodically reinitializing detection for failed services.
 */
@Singleton
public class ServiceDetector {

    private static final Long PROVIDE_ENDPOINTS_TIMEOUT_SECONDS = 10L;
    private static final Long RECHECK_FAILED_URIS_TIMEOUT_SECONDS = 300L;

    @Resource
    ManagedScheduledExecutorService managedExecutorService;

    @Inject
    Logger logger;

    @Inject
    private ProvideEndpointsScheduledTask provideEndpointsScheduledTask;

    @Inject
    private RecheckFailedUrisScheduledTask recheckFailedUrisScheduledTask;

    private boolean pollingStarted;

    @Lock(WRITE)
    public void pollForServices() {
        if (pollingStarted) {
            throw new UnsupportedOperationException("Polling for services already started");
        }

        try {
            managedExecutorService.scheduleWithFixedDelay(provideEndpointsScheduledTask, 0L,
                    PROVIDE_ENDPOINTS_TIMEOUT_SECONDS, SECONDS);
            managedExecutorService.scheduleWithFixedDelay(recheckFailedUrisScheduledTask,
                    RECHECK_FAILED_URIS_TIMEOUT_SECONDS, RECHECK_FAILED_URIS_TIMEOUT_SECONDS, SECONDS);

            pollingStarted = true;
        } catch (RejectedExecutionException e) {
            logger.e("Application failed to start properly. Service polling is disabled.", e);
            throw e;
        }
    }
}
