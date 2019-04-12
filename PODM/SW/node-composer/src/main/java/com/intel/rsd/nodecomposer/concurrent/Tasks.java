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

import com.google.common.util.concurrent.ListeningExecutorService;

import java.time.Duration;
import java.util.concurrent.Callable;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.CompletionException;
import java.util.concurrent.Executor;
import java.util.concurrent.ScheduledExecutorService;

import static com.google.common.util.concurrent.MoreExecutors.newDirectExecutorService;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.Thread.sleep;
import static java.time.Duration.ZERO;
import static java.util.concurrent.CompletableFuture.supplyAsync;
import static java.util.concurrent.TimeUnit.MILLISECONDS;

public final class Tasks {
    private Tasks() {
    }

    public static TaskScheduler newCurrentThreadTaskScheduler() {
        return new CurrentThreadTaskScheduler();
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private static final class CurrentThreadTaskScheduler implements TaskScheduler {
        final ListeningExecutorService executorService = newDirectExecutorService();

        @Override
        public <T> CompletableFuture<T> schedule(Callable<T> callable, Duration delay) {
            requiresNonNull(callable, "callable");
            requiresNonNull(delay, "delay");

            return supplyAsync(() -> {
                try {
                    sleep(delay.toMillis());
                    return callable.call();
                } catch (Exception e) {
                    throw new CompletionException(e);
                }
            }, executorService);
        }

        @Override
        public <T> CompletableFuture<T> submit(Callable<T> callable) {
            return schedule(callable, ZERO);
        }
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private abstract static class TaskSchedulerImpl extends TaskSubmitterImpl implements TaskScheduler {
        @Override
        public <T> CompletableFuture<T> schedule(Callable<T> callable, Duration delay) {
            requiresNonNull(callable, "callable");
            requiresNonNull(delay, "delay");

            CompletableFuture<Void> afterDelay = new CompletableFuture<>();
            getExecutor().schedule(() -> afterDelay.complete(null), delay.toMillis(), MILLISECONDS);

            return afterDelay.thenApply(ignored -> {
                try {
                    return callable.call();
                } catch (Exception e) {
                    throw new CompletionException(e);
                }
            });
        }

        @Override
        protected abstract ScheduledExecutorService getExecutor();
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private abstract static class TaskSubmitterImpl implements TaskSubmitter {
        @Override
        public <T> CompletableFuture<T> submit(Callable<T> callable) {
            requiresNonNull(callable, "callable");

            return supplyAsync(() -> {
                try {
                    return callable.call();
                } catch (Exception e) {
                    throw new CompletionException(e);
                }
            }, getExecutor());
        }

        protected abstract Executor getExecutor();
    }
}
