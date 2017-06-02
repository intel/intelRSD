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

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.Status;
import com.intel.podm.discovery.external.event.EventSubscriberCreator;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;

import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static com.intel.podm.common.types.State.IN_TEST;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ExternalServiceSanitizer {
    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Inject
    private EventSubscriberCreator eventSubscriberCreator;

    @Inject
    private ExternalServiceUpdater externalServiceUpdater;

    @Transactional(REQUIRES_NEW)
    public void sanitizeExistingServices() {
        getAllServices().forEach(this::delegalizeDiscoverables);
    }

    @Transactional(REQUIRES_NEW)
    public void scheduleServicesDiscovery() {
        for (ExternalService service : getAllServices()) {
            discoveryScheduler.scheduleServiceDiscoveryWithEventServiceSubscription(service.getUuid());
        }
    }

    private void delegalizeDiscoverables(ExternalService service) {
        service.getAllOwnedEntities().forEach(discoverable -> discoverable.setStatus(new Status(IN_TEST, null, null)));
    }

    private Collection<ExternalService> getAllServices() {
        // Exclude DeepDiscovery LUI image type
        return externalServiceDao.getExternalServicesByServicesTypes(RMM, PSME, RSS);
    }
}
