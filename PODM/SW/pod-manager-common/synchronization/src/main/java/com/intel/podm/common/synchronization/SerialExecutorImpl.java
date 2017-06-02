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

package com.intel.podm.common.synchronization;

import com.intel.podm.common.logger.Logger;

import java.time.Duration;
import java.util.concurrent.CancellationException;
import java.util.concurrent.CompletableFuture;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static java.util.concurrent.CompletableFuture.completedFuture;
import static java.util.concurrent.TimeUnit.MILLISECONDS;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class SerialExecutorImpl implements SerialExecutor {
    private static final Logger LOG = getLogger(SerialExecutorImpl.class);

    private final ExecutorService delegate;
    private final Object asyncExecutorSynchronizer = new Object();
    private final Object identity;
    private final TaskExecutorProvider taskExecutorProvider;
    private final InBox inBox;
    private final CompletableFutureWrapper running = new CompletableFutureWrapper();

    SerialExecutorImpl(final Object identity, final ExecutorService executor, final TaskExecutorProvider taskExecutorProvider) {
        this.identity = identity;
        this.delegate = executor;
        this.taskExecutorProvider = taskExecutorProvider;
        this.inBox = new InBox(identity);
    }

    @Override
    public Object getIdentity() {
        return identity;
    }

    @Override
    public synchronized <E extends Exception, R> R executeSync(Duration timeToWaitFor, ThrowingCallable<R, E> task) throws TimeoutException, E {
        return execute(timeToWaitFor, taskExecutorProvider.getExecutableExecutor(), task);
    }

    @Override
    public synchronized void executeSync(Duration timeToWaitFor, Runnable task) throws TimeoutException {
        execute(timeToWaitFor, taskExecutorProvider.getRunnableTaskExecutor(), task);
    }

    @Override
    public synchronized <E extends Exception> void executeSync(Duration timeToWaitFor, ThrowingRunnable<E> task) throws TimeoutException, E {
        execute(timeToWaitFor, taskExecutorProvider.getExecutableExecutor(), task);
    }

    @Override
    public void executeAsync(Runnable task) {
        LOG.t("Registering new async task for {}", identity);
        synchronized (asyncExecutorSynchronizer) {
            inBox.add(task);
            if (running.isDone()) {
                scheduleNext();
            }
        }
    }

    private <E extends Exception, R, T> R execute(Duration timeToWaitFor, TaskExecutor<T> executor, T task) throws TimeoutException, E {
        LOG.t("Synchronous task({}) execution has been requested, pausing asynchronous task queue for {}", task, identity);
        inBox.pause();
        try {
            cancelRunnableIfApplicable(running.getCurrentRunnable());
            waitForCurrentlyRunning(timeToWaitFor);
            LOG.t("Running synchronous task({}) for {}", task, identity);
            return executor.execute(task);
        } finally {
            inBox.resume();
            scheduleNext();
        }
    }

    private void cancelRunnableIfApplicable(Runnable runnable) {
        if (runnable instanceof CancelableRunnable) {
            ((CancelableRunnable) runnable).cancel();
        }
    }

    private void waitForCurrentlyRunning(Duration duration) throws TimeoutException {
        if (!running.isDone()) {
            LOG.t("Previous async action for ({}) is still running, pausing sync task for a moment({})", identity, duration);
            try {
                running.get(duration.toMillis(), MILLISECONDS);
            } catch (ExecutionException | CancellationException e) {
                LOG.t("Task has been interrupted or terminated unsuccessfully.");
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    private void scheduleNext() {
        synchronized (asyncExecutorSynchronizer) {
            if (running.isDone()) {
                Runnable runnable = inBox.poll();
                if (runnable != null) {
                    LOG.t("Requesting execution of next ASYNC operation({}) for ({})", runnable, identity);
                    running.runAsyncAndThenRunAsync(runnable, delegate, this::scheduleNext);
                }
            }
        }
    }

    private static final class CompletableFutureWrapper {
        private CompletableFuture<?> running = completedFuture(null);
        private Runnable runnable;

        public boolean isDone() {
            return running.isDone();
        }

        public Object get(long timeout, TimeUnit unit) throws InterruptedException, ExecutionException, TimeoutException {
            return running.get(timeout, unit);
        }

        public synchronized void runAsyncAndThenRunAsync(Runnable runnable, Executor executor, Runnable action) {
            this.runnable = runnable;
            this.running = CompletableFuture.runAsync(runnable, executor);
            this.running.thenRunAsync(action);
        }

        public synchronized Runnable getCurrentRunnable() {
            return runnable;
        }
    }
}
