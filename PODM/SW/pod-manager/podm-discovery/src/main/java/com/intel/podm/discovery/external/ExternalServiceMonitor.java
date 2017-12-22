/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.discovery.external.event.EventSubscriptionMonitor;

import javax.ejb.AccessTimeout;
import javax.ejb.Lock;
import javax.ejb.Singleton;
import javax.enterprise.inject.spi.BeanManager;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static com.intel.podm.discovery.external.ExternalServiceMonitoringEvent.externalServiceMonitoringStartedEvent;
import static com.intel.podm.discovery.external.ExternalServiceMonitoringEvent.externalServiceMonitoringStoppedEvent;
import static java.util.concurrent.TimeUnit.SECONDS;
import static javax.ejb.LockType.WRITE;
import static javax.transaction.Transactional.TxType.SUPPORTS;

@Singleton
public class ExternalServiceMonitor {
    @Inject
    private ExternalServiceRepository externalServiceRepository;

    @Inject
    private ScheduledDiscoveryManager scheduledDiscoveryManager;

    @Inject
    private EventSubscriptionMonitor eventSubscriptionMonitor;

    @Inject
    private BeanManager beanManager;

    /**
     * LockType.WRITE used due to concurrent access to discovery manager and subscription monitor.
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void monitorService(UUID serviceUuid) {
        ExternalService service = externalServiceRepository.find(serviceUuid);
        if (service.isEventingAvailable()) {
            eventSubscriptionMonitor.monitorService(serviceUuid);
            scheduledDiscoveryManager.cancelDiscovery(serviceUuid);
            scheduledDiscoveryManager.scheduleDiscovery(serviceUuid, service.getServiceType());
        } else {
            scheduledDiscoveryManager.scheduleDiscovery(serviceUuid, service.getServiceType());
        }
        beanManager.fireEvent(externalServiceMonitoringStartedEvent(serviceUuid));
    }

    /**
     * LockType.WRITE used due to concurrent access to discovery manager and subscription monitor.
     */
    @Lock(WRITE)
    @Transactional(SUPPORTS)
    @AccessTimeout(value = 5, unit = SECONDS)
    public void stopMonitoringOfService(UUID serviceUuid) {
        eventSubscriptionMonitor.cancelMonitoring(serviceUuid);
        scheduledDiscoveryManager.cancelDiscovery(serviceUuid);
        beanManager.fireEvent(externalServiceMonitoringStoppedEvent(serviceUuid));
    }
}
