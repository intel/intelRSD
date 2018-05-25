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

import com.codahale.metrics.MetricRegistry;
import com.codahale.metrics.SharedMetricRegistries;

import javax.annotation.PreDestroy;
import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.management.InstanceAlreadyExistsException;
import javax.management.InstanceNotFoundException;
import javax.management.MBeanRegistrationException;
import javax.management.MBeanServer;
import javax.management.MalformedObjectNameException;
import javax.management.NotCompliantMBeanException;
import javax.management.ObjectName;
import javax.transaction.Transactional;
import java.lang.management.ManagementFactory;
import java.util.HashSet;
import java.util.Set;

import static com.codahale.metrics.JmxReporter.forRegistry;
import static com.codahale.metrics.SharedMetricRegistries.getOrCreate;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.SUPPORTS;

@Singleton
public class MetricRegistryFactory {

    private final MBeanServer mBeanServer = ManagementFactory.getPlatformMBeanServer();

    private final Set<ObjectName> registeredMBeans = new HashSet<>();

    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public synchronized MetricRegistry getOrRegisterNew(String domain) {
        if (!SharedMetricRegistries.names().contains(domain)) {
            MetricRegistry metricRegistry = getOrCreate(domain);
            try {
                MetricActivator activator = createMetricActivator(domain, metricRegistry);
                ObjectName objectName = new ObjectName(domain, "Configuration", "Activator");
                mBeanServer.registerMBean(activator, objectName);
                registeredMBeans.add(objectName);
            } catch (MalformedObjectNameException | NotCompliantMBeanException | InstanceAlreadyExistsException | MBeanRegistrationException e) {
                throw new IllegalStateException("Cannot register MBeans defined by metric activator", e);
            }
        }
        return getOrCreate(domain);
    }

    @PreDestroy
    private void unregisterMBeans() {
        try {
            for (ObjectName name : registeredMBeans) {
                mBeanServer.unregisterMBean(name);
            }
        } catch (MBeanRegistrationException | InstanceNotFoundException e) {
            throw new IllegalStateException("Cannot unregister registered metric activator MBeans", e);
        }
    }

    private static MetricActivator createMetricActivator(String domain, MetricRegistry metricRegistry) {
        MetricReporter reporter = new ManagedMetricReporterImpl(forRegistry(metricRegistry).inDomain(domain).build());
        return new MetricActivator(reporter);
    }
}
