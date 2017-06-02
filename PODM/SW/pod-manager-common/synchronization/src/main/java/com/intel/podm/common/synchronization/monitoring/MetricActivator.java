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

import javax.annotation.PostConstruct;
import javax.annotation.PreDestroy;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;
import javax.inject.Named;
import javax.management.InstanceAlreadyExistsException;
import javax.management.InstanceNotFoundException;
import javax.management.MBeanRegistrationException;
import javax.management.MBeanServer;
import javax.management.MalformedObjectNameException;
import javax.management.NotCompliantMBeanException;
import javax.management.ObjectName;
import java.lang.management.ManagementFactory;

@Singleton
@Startup
public class MetricActivator implements MetricActivatorMBean {

    @Inject
    @Named("application-executors-jms-reporter")
    private MetricReporter applicationExecutorsReporter;

    private MBeanServer platformMBeanServer;

    private ObjectName metricsActivatorObjectName;

    @PostConstruct
    private void registerMBeans() {
        try {
            metricsActivatorObjectName = new ObjectName(METRICS_DOMAIN, "Configuration", "Activator");
            platformMBeanServer = ManagementFactory.getPlatformMBeanServer();
            platformMBeanServer.registerMBean(this, metricsActivatorObjectName);
        } catch (MalformedObjectNameException | NotCompliantMBeanException | InstanceAlreadyExistsException | MBeanRegistrationException e) {
            throw new IllegalStateException("Cannot register mbeans defined by metric activator", e);
        }
    }

    @PreDestroy
    private void unregisterMBeans() {
        try {
            platformMBeanServer.unregisterMBean(this.metricsActivatorObjectName);
        } catch (MBeanRegistrationException | InstanceNotFoundException e) {
            throw new IllegalStateException("Cannot unregister registered metric activator mbeans", e);
        }
    }

    @Override
    public void enableMetricsForApplicationExecutors() {
        applicationExecutorsReporter.start();
    }

    @Override
    public void disableMetricsForApplicationExecutors() {
        applicationExecutorsReporter.stop();
    }
}
