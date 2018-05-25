/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import static java.lang.String.format;
import static java.util.concurrent.CompletableFuture.completedFuture;
import static java.util.concurrent.CompletableFuture.runAsync;
import static java.util.concurrent.TimeUnit.MILLISECONDS;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class SerialExecutorImpl implements SerialExecutor {
    private static final Logger LOG = getLogger(SerialExecutorImpl.class);

    private final ExecutorService delegate;
    private final Object asyncExecutorSynchronizer = new Object();
    private final Object identity;
    private final Locker syncLocker = new Locker();
    private final InBox inBox;
    private final CompletableFutureWrapper running = new CompletableFutureWrapper();

    SerialExecutorImpl(final Object identity, final ExecutorService executor) {
        this.identity = identity;
        this.delegate = executor;
        this.inBox = new InBox(identity);
    }

    @Override
    public Object getIdentity() {
        return identity;
    }

    @Override
    public <E extends Exception, R> R executeSync(Duration timeToWaitFor, Executable<E, R> executable) throws TimeoutException, E {
        syncLocker.lockOrThrow(timeToWaitFor);
        try {
            return execute(timeToWaitFor, executable);
        } finally {
            syncLocker.unlockIfHeldByCurrentThread();
        }
    }

    @Override
    public void executeAsync(Runnable task) {
        LOG.t("Registering new async task {} for {}", task, identity);
        synchronized (asyncExecutorSynchronizer) {
            inBox.add(task);
            if (running.isDone()) {
                scheduleNext();
            }
        }
    }

    @SuppressWarnings({"checkstyle:IllegalCatch"})
    private <E extends Exception, R> R execute(Duration timeToWaitFor, Executable<E, R> task) throws TimeoutException, E {
        LOG.t("Synchronous task({}) execution has been requested, pausing asynchronous task queue for {}", task, identity);
        inBox.pause();
        try {
            cancelRunnableIfApplicable(running.getCurrentRunnable());
            waitForCurrentlyRunning(timeToWaitFor);
            LOG.d("Coordinated Task (sync) {}/{} - started", task, identity);
            R execute = task.execute();
            LOG.d("Coordinated Task (sync) {}/{} - ended", task, identity);
            return execute;
        } catch (Throwable throwable) {
            LOG.e(format("Task(%s) run for %s finished with exception.", task, identity), throwable);
            LOG.d("Task coordinator queue dump: {}", inBox.print());
            throw throwable;
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
                    running.runAsyncAndThenRunAsync(runnable, delegate, this::scheduleNext, identity);
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

        public synchronized void runAsyncAndThenRunAsync(Runnable runnable, Executor executor, Runnable action, Object identity) {
            this.runnable = runnable;
            this.running = runAsync(logAndRun(runnable, identity), executor);
            this.running.thenRunAsync(action);
            this.running.exceptionally(throwable -> {
                LOG.e(format("Asynchronous operation (%s) execution finished with exception", runnable), throwable);
                return null;
            });
        }

        private Runnable logAndRun(Runnable runnable, Object identity) {
            return () -> {
                LOG.d("Coordinated Task (async) {}/{} - started", runnable, identity);
                runnable.run();
                LOG.d("Coordinated Task (async) {}/{} - ended", runnable, identity);
            };
        }

        public synchronized Runnable getCurrentRunnable() {
            return runnable;
        }
    }
}
