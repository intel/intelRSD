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

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.ServiceType;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.time.Duration;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.ExternalService.GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES;
import static com.intel.podm.business.entities.redfish.ExternalService.GET_EXTERNAL_SERVICE_BY_UUID;
import static com.intel.podm.common.utils.IterableHelper.getUniqueValueFromSingletonListOrNull;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class ExternalServiceDao extends Dao<ExternalService> {

    @Transactional(MANDATORY)
    public ExternalService tryGetUniqueExternalServiceByUuid(UUID uuid) {
        TypedQuery<ExternalService> query = entityManager.createNamedQuery(GET_EXTERNAL_SERVICE_BY_UUID, ExternalService.class);
        query.setParameter("uuid", uuid);
        return getUniqueValueFromSingletonListOrNull(query.getResultList());
    }

    @Transactional(MANDATORY)
    public List<ExternalService> getExternalServicesByServicesTypes(Set<ServiceType> serviceTypes) {
        if (serviceTypes == null || serviceTypes.isEmpty()) {
            return emptyList();
        }

        TypedQuery<ExternalService> query = entityManager.createNamedQuery(GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES, ExternalService.class);
        query.setParameter("serviceTypes", serviceTypes);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<ExternalService> getAllUnreachableLongerThan(Duration duration) {
        return findAll().stream()
            .filter(es -> es.isUnreachableLongerThan(duration))
            .collect(toList());
    }
}
