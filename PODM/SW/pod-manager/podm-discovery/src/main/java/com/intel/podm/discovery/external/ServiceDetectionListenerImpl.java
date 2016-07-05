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
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.detection.ServiceDetectionListener;
import com.intel.podm.services.detection.ServiceEndpoint;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.common.types.ServiceType.RSS;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@ApplicationScoped
public class ServiceDetectionListenerImpl implements ServiceDetectionListener {
    @Inject
    private Logger logger;

    @Inject
    private DiscoveryScheduler discoveryScheduler;

    @Inject
    private ExternalServiceUpdater externalServiceUpdater;

    @Inject
    private ExternalServiceRepository externalServiceRepository;

    @Inject
    private StorageGuard storageGuard;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Override
    public void onServiceDetected(ServiceEndpoint serviceEndpoint) {
        logger.i("Discovery {} started", serviceEndpoint);
        externalServiceUpdater.updateExternalService(serviceEndpoint);

        switch (serviceEndpoint.getServiceType()) {
            case LUI:
                discoveryScheduler.triggerDiscovery(serviceEndpoint.getServiceUuid());
                break;
            case RSS:
                storageGuard.protectStorageEndpoint(serviceEndpoint);
                discoveryScheduler.scheduleServiceDiscoveryWithEventServiceSubscription(serviceEndpoint.getServiceUuid());
                break;
            default:
                discoveryScheduler.scheduleServiceDiscoveryWithEventServiceSubscription(serviceEndpoint.getServiceUuid());
                break;
        }
    }

    @Override
    @Transactional(REQUIRES_NEW)
    @NumberOfRetriesOnRollback(3)
    @Interceptors(RetryOnRollbackInterceptor.class)
    public void onServiceRemoved(ServiceEndpoint serviceEndpoint) {
        UUID serviceUuid = serviceEndpoint.getServiceUuid();
        ExternalService serviceToDelete = externalServiceRepository.findOrNull(serviceUuid);

        if (serviceToDelete == null) {
            logger.d("Trying to remove service with UUID: {}, which does not exist", serviceUuid);
            return;
        }

        if (Objects.equals(serviceToDelete.getBaseUri(), serviceEndpoint.getEndpointUri())) {
            logger.i("Removing service {}", serviceToDelete);

            if (serviceEndpoint.getServiceType().equals(RSS)) {
                storageGuard.removeProtectionForStorageEndpoint(serviceEndpoint);
            }

            discoveryScheduler.cancel(serviceUuid);
            externalServiceDao.remove(serviceToDelete);
            logger.i("Service {} removed", serviceToDelete);
        } else {
            logger.w("Trying to remove service {} that has an updated endpoint URI", serviceUuid);
        }
    }
}
