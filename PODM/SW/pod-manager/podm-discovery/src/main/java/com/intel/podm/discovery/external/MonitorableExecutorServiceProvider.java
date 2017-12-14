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

package com.intel.podm.discovery.external;

import com.codahale.metrics.InstrumentedScheduledExecutorService;
import com.codahale.metrics.MetricRegistry;
import com.intel.podm.common.synchronization.monitoring.MetricRegistryFactory;

import javax.annotation.Resource;
import javax.enterprise.concurrent.ManagedScheduledExecutorService;
import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.inject.Produces;
import javax.inject.Inject;
import javax.inject.Named;
import java.util.concurrent.ScheduledExecutorService;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.EVENT_SUBSCRIPTION_TASK_EXECUTOR;
import static com.intel.podm.common.enterprise.utils.beans.JndiNames.SYNCHRONIZED_TASK_EXECUTOR;

@ApplicationScoped
public class MonitorableExecutorServiceProvider {
    @Resource(lookup = "java:/" + EVENT_SUBSCRIPTION_TASK_EXECUTOR)
    private ScheduledExecutorService eventSubscriptionTaskExecutor;

    @Resource(lookup = "java:/" + SYNCHRONIZED_TASK_EXECUTOR)
    private ManagedScheduledExecutorService synchronizedTaskExecutor;

    @Inject
    private MetricRegistryFactory metricRegistryFactory;

    @Produces
    @Named(SYNCHRONIZED_TASK_EXECUTOR)
    @ApplicationScoped
    public ScheduledExecutorService getDiscoveryTaskExecutor() {
        return new InstrumentedScheduledExecutorService(synchronizedTaskExecutor, getMetricRegistry(), SYNCHRONIZED_TASK_EXECUTOR);
    }

    @Produces
    @Named(EVENT_SUBSCRIPTION_TASK_EXECUTOR)
    @ApplicationScoped
    public ScheduledExecutorService getEventSubscriptionTaskExecutor() {
        return new InstrumentedScheduledExecutorService(eventSubscriptionTaskExecutor, getMetricRegistry(), EVENT_SUBSCRIPTION_TASK_EXECUTOR);
    }

    private MetricRegistry getMetricRegistry() {
        return metricRegistryFactory.getOrRegisterNew("PodM.Misc");
    }
}
