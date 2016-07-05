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

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.services.detection.ServiceEndpoint;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Objects;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ExternalServiceUpdater {

    @Inject
    private Logger logger;

    @Inject
    private ExternalServiceRepository repository;

    @Transactional(REQUIRES_NEW)
    public void updateExternalService(ServiceEndpoint serviceEndpoint) {
        ExternalService service = repository.findOrNull(serviceEndpoint.getServiceUuid());
        if (service == null) {
            service = repository.create(serviceEndpoint);
            logger.t("New external service created for {}", serviceEndpoint);
        }

        if (!Objects.equals(service.getBaseUri(), serviceEndpoint.getEndpointUri())) {
            logger.t("Service's URI for {} was updated to {}", service, serviceEndpoint.getEndpointUri());
        }

        service.setBaseUri(serviceEndpoint.getEndpointUri());
    }

    @Transactional(REQUIRES_NEW)
    public void updateExternalServiceUuid(UUID oldUuid, UUID newUuid) {
        ExternalService service = repository.find(oldUuid);
        service.setUuid(newUuid);
    }
}
