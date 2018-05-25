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

import java.util.ArrayDeque;
import java.util.function.Supplier;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;

class InBox {

    private static final Logger LOG = getLogger(InBox.class);

    private Object identity;

    private ArrayDeque<Runnable> queue = new ArrayDeque<>();

    private Supplier<Runnable> queueTaskSupplier = () -> queue.poll();

    private Supplier<Runnable> noTaskSupplier = () -> null;

    private Supplier<Runnable> selectedSupplier = queueTaskSupplier;

    InBox(Object identity) {
        this.identity = identity;
    }

    public synchronized void pause() {
        selectedSupplier = noTaskSupplier;
        LOG.t("Asynchronous tasks paused for {}", identity);
    }

    public synchronized void resume() {
        selectedSupplier = queueTaskSupplier;
        LOG.t("Asynchronous tasks resumed for {}", identity);
    }

    Runnable poll() {
        return selectedSupplier.get();
    }

    void add(Runnable task) {
        if (task.equals(queue.peekLast())) {
            LOG.i("Task ({}) is already enqueued, ignoring...", task);
        } else {
            LOG.t("Current InBox size: {} for service: {}", queue.size(), identity.toString());
            queue.offer(task);
        }
    }

    public String print() {
        return queue.toString();
    }
}
