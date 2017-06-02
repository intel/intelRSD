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

import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.dao.ExternalServiceDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.ServiceType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ExternalServiceRepository {
    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private Logger logger;

    @Transactional(MANDATORY)
    public ExternalService find(UUID uuid) {
        ExternalService service = findOrNull(uuid);

        if (service == null) {
            String msg = format("there is no service with UUID '%s'", uuid);
            throw new IllegalStateException(msg);
        }

        return service;
    }

    @Transactional(MANDATORY)
    public ExternalService findOrNull(UUID uuid) {
        requiresNonNull(uuid, "uuid");

        try {
            return externalServiceDao.getExternalServiceByUuid(uuid);
        } catch (NonUniqueResultException e) {
            return null;
        }
    }

    @Transactional(MANDATORY)
    public ExternalService create(ServiceEndpoint endpoint) {
        UUID uuid = endpoint.getServiceUuid();

        requiresNonNull(uuid, "uuid");

        if (findOrNull(uuid) != null) {
            String msg = format("service with UUID '%s' exists", uuid);
            throw new IllegalStateException(msg);
        }

        ExternalService service = externalServiceDao.create();
        service.setUuid(uuid);
        service.setServiceType(endpoint.getServiceType());
        return service;
    }

    @Transactional(MANDATORY)
    public Collection<ExternalService> getAllByType(ServiceType serviceType) {
        return externalServiceDao.getExternalServicesByServicesTypes(serviceType);
    }
}
