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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.enterprise.utils.logger.ServiceLifecycle;
import com.intel.podm.common.logger.ServiceLifecycleLogger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ExternalServiceConfig;
import com.intel.podm.discovery.ServiceExplorer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.time.Duration;
import java.util.List;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@SuppressWarnings("checkstyle:IllegalCatch")
public class ServiceRemovalTask implements Runnable {

    @Inject
    private Remover remover;

    @Inject
    @ServiceLifecycle
    private ServiceLifecycleLogger logger;

    @Override

    public void run() {
        try {
            remover.remove();
        } catch (Exception e) {
            logger.i("Exception({}) caught during performing ServiceRemovalTask: {}", e.getClass(), e.getMessage());
        }
    }

    @Override
    public String toString() {
        return getClass().getSimpleName();
    }

    @Dependent
    static class Remover {

        @Inject
        @ServiceLifecycle
        private ServiceLifecycleLogger logger;

        @Inject
        private ServiceExplorer serviceExplorer;

        @Inject
        private ExternalServiceDao externalServiceDao;

        @Inject
        @Config(refreshable = true)
        private Holder<ExternalServiceConfig> config;

        @Transactional(REQUIRES_NEW)
        public void remove() {
            logger.t("checking whether unreachable services should be evicted");

            Duration howLongCanBeRetained = config.get().getServiceRemovalDelay();
            List<ExternalService> unreachableServices = findUnreachableServices(howLongCanBeRetained);

            unreachableServices.stream()
                .peek(service -> logger.lifecycleInfo("{} is unreachable longer than {} - will be evicted.", service, howLongCanBeRetained))
                .forEach(this::removeService);
        }

        private List<ExternalService> findUnreachableServices(Duration howLongCanBeRetained) {
            return externalServiceDao.getAllUnreachableLongerThan(howLongCanBeRetained);
        }

        private void removeService(ExternalService service) {
            serviceExplorer.forgetService(service.getUuid());
            externalServiceDao.remove(service);
        }
    }
}
