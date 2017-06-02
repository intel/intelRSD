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

public interface TaskCoordinator {
    void registerAsync(Object synchronizationKey, Runnable task);

    <E extends Exception> void runThrowing(Object synchronizationKey, Duration timeToWaitFor, ThrowingRunnable<E> task) throws TimeoutException, E;

    <E extends Exception> void runThrowing(Object synchronizationKey, ThrowingRunnable<E> task) throws TimeoutException, E;

    void run(Object synchronizationKey, Runnable task) throws TimeoutException;

    <E extends Exception, R> R call(Object synchronizationKey, ThrowingCallable<R, E> task) throws TimeoutException, E;

    void scheduleWithFixedDelay(Object synchronizationKey, Runnable task, Duration initialDelay, Duration delay);
}
