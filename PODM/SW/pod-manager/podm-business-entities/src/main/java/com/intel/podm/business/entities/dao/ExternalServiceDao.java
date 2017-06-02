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


import com.intel.podm.business.entities.IdFromUriGenerator;
import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.common.types.ServiceType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.net.URI;
import java.time.Duration;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.ExternalService.GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES;
import static com.intel.podm.business.entities.redfish.ExternalService.GET_EXTERNAL_SERVICE_BY_UUID;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ExternalServiceDao extends Dao<ExternalService> {

    @Inject
    IdFromUriGenerator idFromUriGenerator;

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T findOrCreateEntity(ExternalService externalService, URI uri, Class<T> clazz) {
        for (DiscoverableEntity entity : externalService.getOwned(DiscoverableEntity.class)) {
            URI sourceUri = entity.getSourceUri();

            if (Objects.equals(uri, sourceUri)) {
                return (T) entity;
            }
        }

        return createEntity(externalService, uri, clazz);
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T createEntity(ExternalService externalService, URI uri, Class<T> clazz) {
        T entity = create(clazz);
        entity.setSourceUri(uri);
        externalService.addOwned(entity);
        entity.setId(idFromUriGenerator.getIdFromUri(uri, externalService.getId().getValue()));
        return entity;
    }

    @Transactional(MANDATORY)
    public ExternalService getExternalServiceByUuid(UUID uuid) throws NonUniqueResultException {
        TypedQuery<ExternalService> query = entityManager.createNamedQuery(GET_EXTERNAL_SERVICE_BY_UUID, ExternalService.class);
        query.setParameter("uuid", uuid);
        return singleEntityOrNull(query.getResultList());
    }

    @Transactional(MANDATORY)
    public List<ExternalService> getExternalServicesByServicesTypes(ServiceType... serviceTypes) {
        if (serviceTypes == null || serviceTypes.length == 0) {
            return emptyList();
        }

        TypedQuery<ExternalService> query = entityManager.createNamedQuery(GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES, ExternalService.class);
        query.setParameter("serviceTypes", asList(serviceTypes));
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<ExternalService> getAllUnreachableLongerThan(Duration duration) {
        return findAll().stream()
            .filter(es -> es.isUnreachableLongerThan(duration))
            .collect(toList());
    }
}
