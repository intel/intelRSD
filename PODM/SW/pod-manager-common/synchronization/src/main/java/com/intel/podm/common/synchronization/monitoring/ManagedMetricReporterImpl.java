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

package com.intel.podm.common.synchronization.monitoring;

import com.codahale.metrics.JmxReporter;

class ManagedMetricReporterImpl implements MetricReporter {

    private JmxReporter underlyingReporter;

    private boolean started;

    ManagedMetricReporterImpl(JmxReporter underlyingReporter) {
        this.underlyingReporter = underlyingReporter;
    }

    @Override
    public synchronized void start() {
        if (!started) {
            underlyingReporter.start();
            started = true;
        }
    }

    @Override
    public synchronized void stop() {
        if (started) {
            underlyingReporter.stop();
            started = false;
        }
    }
}
