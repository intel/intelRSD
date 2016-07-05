/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.events.EventSubscriptionManager;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.discovery.external.event.EventSubscriberCreator;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;

@Dependent
public class ExternalServiceSanitizer {

    @Inject
    private ExternalServiceDao serviceDao;

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Inject
    private ExternalServiceDiscoverablesDelegalizer serviceDiscoverablesDelegalizer;

    @Inject
    private EventSubscriberCreator eventSubscriberCreator;

    @Inject
    private ExternalServiceUpdater externalServiceUpdater;

    @Inject
    private Logger logger;

    public void sanitizeExistingServices() {
        sanitizeServicesOfType(RMM);
        sanitizeServicesOfType(PSME);
        sanitizeServicesOfType(RSS);
    }

    public void sanitizeService(UUID oldUuid, UUID newUuid) throws ExternalServiceApiActionException, ExternalServiceApiReaderException {
        externalServiceUpdater.updateExternalServiceUuid(oldUuid, newUuid);
        sanitizeSubscriptions(oldUuid, newUuid);
    }

    private void sanitizeSubscriptions(UUID oldUuid, UUID newUuid) throws ExternalServiceApiActionException, ExternalServiceApiReaderException {
        discoveryScheduler.cancel(oldUuid);
        EventSubscriptionManager eventSubscriptionManager = eventSubscriberCreator.create(newUuid);
        eventSubscriptionManager.deleteSubscription();
        discoveryScheduler.subscribeAndScheduleEventService(newUuid);
    }

    private void sanitizeServicesOfType(ServiceType serviceType) {
        Collection<ExternalService> externalServices = serviceDao.getExternalServicesByServiceType(serviceType);

        externalServices.forEach(serviceDiscoverablesDelegalizer::delegalizeDiscoverables);
        externalServices.stream()
                .map(ExternalService::getUuid)
                .forEach(discoveryScheduler::scheduleServiceDiscoveryWithEventServiceSubscription);
    }
}
