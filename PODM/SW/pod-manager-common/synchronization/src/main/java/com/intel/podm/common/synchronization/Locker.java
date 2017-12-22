/*
 * Copyright (c) 2017 Intel Corporation
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
import java.util.concurrent.locks.ReentrantLock;

import static com.google.common.base.MoreObjects.toStringHelper;
import static java.util.concurrent.TimeUnit.MILLISECONDS;

class Locker {
    private final ReentrantLock lock = new ReentrantLock(true);

    public void lockOrThrow(Duration timeToWait) throws TimeoutException {
        try {
            if (!lock.tryLock(timeToWait.toMillis(), MILLISECONDS)) {
                throw new TimeoutException("Could not acquire lock for " + timeToWait.getSeconds() + " seconds.");
            }
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
    }

    public void unlockIfHeldByCurrentThread() {
        if (lock.isHeldByCurrentThread()) {
            lock.unlock();
        }
    }

    @Override
    public String toString() {
        return toStringHelper(this)
            .add("lock", lock)
            .toString();
    }
}
