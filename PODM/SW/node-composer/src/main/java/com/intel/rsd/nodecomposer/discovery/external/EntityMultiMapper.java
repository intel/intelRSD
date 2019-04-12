/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.mappers.Mapper;
import com.intel.rsd.nodecomposer.mappers.MapperFinder;
import com.intel.rsd.nodecomposer.persistence.dao.DiscoverableEntityDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static java.util.Optional.empty;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class EntityMultiMapper {
    private final DiscoverableEntityDao discoverableEntityDao;
    private final GenericDao genericDao;
    private final MapperFinder mapperFinder;

    @Autowired
    EntityMultiMapper(DiscoverableEntityDao discoverableEntityDao, GenericDao genericDao, MapperFinder mapperFinder) {
        this.discoverableEntityDao = discoverableEntityDao;
        this.genericDao = genericDao;
        this.mapperFinder = mapperFinder;
    }

    @Transactional(MANDATORY)
    public Map<ExternalServiceResource, DiscoverableEntity> map(Collection<ExternalServiceResource> resources) {
        Map<ExternalServiceResource, DiscoverableEntity> result = new HashMap<>();

        for (ExternalServiceResource resource : resources) {
            DiscoverableEntity entity = map(resource);
            result.put(resource, entity);
        }
        deleteStaleEntities(resources);

        return result;
    }

    private void deleteStaleEntities(Collection<ExternalServiceResource> resources) {
        Collection<ODataId> resourceUris = resources.stream()
            .map(externalServiceResource -> oDataIdFromUri(externalServiceResource.getUri()))
            .collect(toSet());

        genericDao.removeAndClear(
            discoverableEntityDao.findAll(),
            discoverableEntity -> !resourceUris.contains(discoverableEntity.getUri())
        );
    }

    @Transactional(MANDATORY)
    public DiscoverableEntity map(ExternalServiceResource resource) {
        return mapperFinder.find(resource)
            .flatMap(mapper -> mapWithMapper(resource, mapper))
            .orElse(null);
    }

    @SuppressWarnings({"unchecked"})
    private Optional<DiscoverableEntity> mapWithMapper(ExternalServiceResource resource, Mapper mapper) {
        try {
            return matchResource(resource, mapper.getTargetClass())
                .map(entity -> {
                    mapper.map(resource, entity);
                    return entity;
                });
        } catch (IllegalStateException e) {
            log.error("Problem while matching resource: '{}'\n{}", resource.getUri(), e.getMessage());
            return empty();
        }
    }

    @SuppressWarnings({"unchecked"})
    private Optional<DiscoverableEntity> matchResource(ExternalServiceResource resource, Class targetClass) {
        return ofNullable(discoverableEntityDao.findOrCreateEntity(resource.getUri(), targetClass));
    }
}
