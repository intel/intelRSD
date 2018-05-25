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

import com.codahale.metrics.MetricRegistry;
import com.codahale.metrics.Timer;

import java.time.Duration;
import java.util.concurrent.TimeoutException;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
class InstrumentedSerialExecutor implements SerialExecutor {
    private Timer duration;
    private SerialExecutor delegate;

    InstrumentedSerialExecutor(SerialExecutor delegate, MetricRegistry metricRegistry) {
        this.delegate = delegate;
        this.duration = metricRegistry.timer("serial-executor-" + delegate.getIdentity() + "-sync-task-duration");
    }

    @Override
    public Object getIdentity() {
        return delegate.getIdentity();
    }

    @Override
    public <E extends Exception, R> R executeSync(Duration timeToWaitFor, Executable<E, R> executable) throws TimeoutException, E {
        final Timer.Context context = duration.time();
        try {
            return delegate.executeSync(timeToWaitFor, executable);
        } finally {
            context.stop();
        }
    }

    @Override
    public void executeAsync(Runnable task) {
        delegate.executeAsync(task);
    }
}
