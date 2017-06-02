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

import java.time.Duration;
import java.util.concurrent.TimeoutException;

/**
 * The executor below serializes the submission of tasks to a second executor.
 */
interface SerialExecutor {
    /**
     * @return identity of this serial executor. Identity should be unique across PodM instance.
     */
    Object getIdentity();

    /**
     * Executes provided {@code task} synchronously.
     * <p>
     * If there is no other task running, provided task will be executed immediately.
     * If any other task is running, current thread will wait until the end of this task (no longer than timeToWaitFor).
     *
     * @param timeToWaitFor accepted timeout
     * @param task task to perform
     * @param <R> type of result which will be returned
     * @return result of synchronous task
     * @throws TimeoutException if previous task is still running, even after applying accepted timeout
     */
    <E extends Exception, R> R executeSync(Duration timeToWaitFor, ThrowingCallable<R, E> task) throws TimeoutException, E;

    void executeSync(Duration timeToWaitFor, Runnable task) throws TimeoutException;

    /**
     * {@link #executeSync(Duration, ThrowingCallable)}
     */
    <E extends Exception> void executeSync(Duration timeToWaitFor, ThrowingRunnable<E> task) throws TimeoutException, E;


    /**
     * Enqueues provided task. Enqueued task will be executed later based on FIFO order.
     * When synchronous task is coming, processing of enqueued asynchronous tasks will be paused,
     * sync task will be executed and finally processing of enqueued tasks will be resumed.
     *
     * @param task task to enqueue
     */
    void executeAsync(Runnable task);
}
