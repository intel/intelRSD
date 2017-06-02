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

package com.intel.podm.discovery.external.event;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.client.api.events.EventSubscriberFactory;
import com.intel.podm.client.api.events.EventSubscriptionManager;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.discovery.external.ExternalServiceRepository;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.EnumSet;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static java.lang.String.format;
import static java.util.EnumSet.of;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class EventSubscriberCreator {
    private static final EnumSet<ServiceType> SUPPORTED_SERVICE_TYPES = of(PSME, RSS, RMM);

    @Inject
    private ExternalServiceRepository repository;

    @Inject
    private EventSubscriberFactory eventSubscriberFactory;

    @Inject
    private EventDataProvider eventDataProvider;

    @Transactional(REQUIRES_NEW)
    public EventSubscriptionManager create(UUID serviceUuid) {
        ExternalService service = repository.find(serviceUuid);
        EventServiceDefinitionImpl eventServiceDefinition = eventDataProvider.createEventServiceDefinition(service);

        ServiceType serviceType = service.getServiceType();
        if (SUPPORTED_SERVICE_TYPES.contains(serviceType)) {
            return eventSubscriberFactory.createEventSubscriberManager(eventServiceDefinition);
        } else {
            String msg = format("Events for service type '%s' are not supported", serviceType);
            throw new UnsupportedOperationException(msg);
        }
    }
}
