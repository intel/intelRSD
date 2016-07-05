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
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;
import com.intel.podm.services.detection.ServiceEndpoint;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;
import java.util.UUID;

import static java.lang.String.format;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ExternalServiceRepository {

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private Logger logger;

    public ExternalService find(UUID uuid) {
        ExternalService service = findOrNull(uuid);

        if (service == null) {
            String msg = format("there is no service with UUID '%s'", uuid);
            throw new IllegalStateException(msg);
        }

        return service;
    }

    public ExternalService findOrNull(UUID uuid) {
        if (uuid == null) {
            throw new IllegalArgumentException("uuid must not be null");
        }

        return externalServiceDao.getExternalServiceByUuid(uuid);
    }

    public ExternalService create(ServiceEndpoint endpoint) {
        UUID uuid = endpoint.getServiceUuid();

        if (uuid == null) {
            throw new IllegalArgumentException("uuid must not be null");
        }

        if (findOrNull(uuid) != null) {
            String msg = format("service with UUID '%s' exists", uuid);
            throw new IllegalStateException(msg);
        }

        ExternalService service = externalServiceDao.create();
        service.setUuid(uuid);
        service.setServiceType(endpoint.getServiceType());
        service.setMacAddress(endpoint.getMacAddress());
        return service;
    }

    public Collection<ExternalService> getAllByType(ServiceType serviceType) {
        return externalServiceDao.getExternalServicesByServiceType(serviceType);
    }

    public Set<UUID> getAllServiceUuids() {
        return externalServiceDao.findAll().stream()
            .map(ExternalService::getUuid)
            .collect(toSet());
    }
}
