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

package com.intel.podm.discovery;

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.common.types.Status;

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.EnumSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.INBAND;
import static com.intel.podm.common.types.ServiceType.PSME;
import static com.intel.podm.common.types.ServiceType.RMM;
import static com.intel.podm.common.types.ServiceType.RSS;
import static com.intel.podm.common.types.State.IN_TEST;
import static java.util.EnumSet.of;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Singleton
@Startup
@DependsOn({"ResourceProvider", "PodStartupDiscovery"})
public class DiscoveryStartup {
    @Inject
    private Logger logger;

    @Inject
    private DiscoverablesDelegalizer delegalizer;

    @Inject
    private ServiceExplorer serviceExplorer;

    @PostConstruct
    private void initialize() {
        logger.d("Sanitizing existing external services");
        Set<UUID> sanitizedServicesUuids = delegalizer.delegalizeExternalServicesResources();

        for (UUID serviceUuid : sanitizedServicesUuids) {
            serviceExplorer.startMonitoringOfService(serviceUuid);
        }
    }

    @Dependent
    public static class DiscoverablesDelegalizer {
        // Exclude DeepDiscovery LUI image type
        private static final EnumSet<ServiceType> MANAGED_TYPES = of(PSME, RSS, RMM, INBAND);

        @Inject
        private ExternalServiceDao externalServiceDao;

        @Transactional(REQUIRES_NEW)
        public Set<UUID> delegalizeExternalServicesResources() {
            List<ExternalService> services = externalServiceDao.getExternalServicesByServicesTypes(MANAGED_TYPES);
            for (ExternalService service : services) {
                service.getOwnedLinks()
                    .stream()
                    .map(entityLink -> entityLink.getDiscoverableEntity())
                    .forEach(discoverableEntity -> discoverableEntity.setStatus(new Status(IN_TEST, null, null)));
            }

            return services.stream()
                .map(ExternalService::getUuid)
                .collect(toSet());
        }
    }
}
