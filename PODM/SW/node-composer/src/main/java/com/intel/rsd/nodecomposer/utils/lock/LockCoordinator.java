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

package com.intel.rsd.nodecomposer.utils.lock;

import lombok.extern.slf4j.Slf4j;

import java.time.Duration;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;
import java.util.concurrent.locks.ReentrantLock;

@Slf4j
final class LockCoordinator {
    private static final Map<Class<?>, ReentrantLock> LOCKS = new HashMap<>();

    private LockCoordinator() {
    }

    static void lockOrThrow(Class<?> clazz, Duration timeToWait) throws TimeoutException {
        try {
            ReentrantLock lock = getLock(clazz);
            long startTime = System.currentTimeMillis();
            if (lock.tryLock(timeToWait.toMillis(), TimeUnit.MILLISECONDS)) {
                long estimatedTime = System.currentTimeMillis() - startTime;
                log.trace("Acquiring lock took " + estimatedTime + " ms.");
            } else {
                throw new TimeoutException("Could not acquire lock for " + timeToWait.toMillis() + " ms.");
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    static void unlockIfHeldByCurrentThread(Class<?> clazz) {
        ReentrantLock lock = getLock(clazz);
        if (lock.isHeldByCurrentThread()) {
            lock.unlock();
        }
    }

    private static synchronized ReentrantLock getLock(Class<?> clazz) {
        return LOCKS.computeIfAbsent(clazz, cl -> new ReentrantLock(true));
    }
}
