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

package com.intel.rsd.services.common.hazelcast;

import lombok.NonNull;
import lombok.RequiredArgsConstructor;
import org.slf4j.Logger;
import org.springframework.context.event.EventListener;
import org.springframework.integration.leader.event.OnGrantedEvent;
import org.springframework.integration.leader.event.OnRevokedEvent;
import org.springframework.scheduling.TaskScheduler;

import java.time.Duration;
import java.util.concurrent.Future;

import static java.util.concurrent.CompletableFuture.completedFuture;
import static org.slf4j.LoggerFactory.getLogger;

/**
 * Base class for tasks that should be scheduled with fixed delay only on Leader Instance.
 */
@RequiredArgsConstructor
public abstract class ScheduledWithFixedDelayOnLeaderTask implements Runnable {
    protected final Logger log = getLogger(getClass());

    @NonNull
    private final TaskScheduler taskScheduler;
    /**
     * Handle to this task scheduled with TaskScheduler. Initialized with completed future to avoid null checking.
     */
    private Future thisTask = completedFuture(null);

    @EventListener(OnGrantedEvent.class)
    protected void start() {
        cancel();

        thisTask = taskScheduler.scheduleWithFixedDelay(this, getFixedDelay().toMillis());
        log.info("{} has been scheduled", this);
    }

    @EventListener(OnRevokedEvent.class)
    protected void cancel() {
        if (thisTask.isDone()) {
            return;
        }

        if (thisTask.cancel(true)) {
            log.info("{} has been cancelled", this);
        } else if (!thisTask.isCancelled()) {
            log.error("{} has not been canceled", this);
        }
    }

    protected abstract Duration getFixedDelay();

    @Override
    public abstract String toString();
}
