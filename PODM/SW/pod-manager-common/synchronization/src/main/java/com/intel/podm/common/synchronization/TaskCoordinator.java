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

import java.time.Duration;
import java.util.UUID;
import java.util.concurrent.TimeoutException;

public interface TaskCoordinator {
    void registerAsync(UUID synchronizationKey, Runnable task);

    <E extends Exception> void run(UUID synchronizationKey, ThrowingRunnable<E> task) throws TimeoutException, E;

    <E extends Exception, R> R call(UUID synchronizationKey, ThrowingCallable<R, E> task) throws TimeoutException, E;

    void scheduleWithFixedDelay(UUID synchronizationKey, Runnable task, Duration initialDelay, Duration delay);
}
