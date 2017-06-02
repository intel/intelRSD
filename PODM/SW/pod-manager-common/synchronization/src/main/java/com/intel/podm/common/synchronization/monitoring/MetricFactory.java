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

package com.intel.podm.common.synchronization.monitoring;

import com.codahale.metrics.MetricRegistry;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Produces;
import javax.inject.Inject;
import javax.inject.Named;

import static com.codahale.metrics.JmxReporter.forRegistry;
import static com.intel.podm.common.synchronization.monitoring.MetricActivatorMBean.METRICS_DOMAIN;

@ApplicationScoped
public class MetricFactory {

    @Inject
    private MetricRegistry metricRegistry;

    @Produces
    @Named("application-executors-jms-reporter")
    @ApplicationScoped
    public MetricReporter createApplicationExecutorReporter() {
        return new ManagedMetricReporterImpl(forRegistry(metricRegistry)
                .filter((name, metric) -> !name.contains("serial-executor"))
                .inDomain(METRICS_DOMAIN).build());
    }

}