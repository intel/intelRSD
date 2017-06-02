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
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.client.api.resources.ExternalServiceResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.discovery.external.matchers.EntityObtainer;
import com.intel.podm.mappers.Mapper;
import com.intel.podm.mappers.MapperFinder;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Optional;

import static com.intel.podm.common.types.ServiceType.LUI;
import static com.intel.podm.common.utils.Contracts.requires;
import static java.util.Objects.nonNull;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Dependent
public class EntityMultiMapper {
    @Inject
    private EntityObtainer entityObtainer;

    @Inject
    private ExternalServiceDao externalServiceDao;

    @Inject
    private GenericDao genericDao;

    @Inject
    private Logger logger;

    @Inject @Config
    private Holder<DiscoveryConfig> config;

    @Inject
    private MapperFinder mapperFinder;

    public Map<ExternalServiceResource, Entity> map(Collection<ExternalServiceResource> resources, ExternalService externalService) {
        requires(nonNull(externalService), "externalService should not be null");

        Map<ExternalServiceResource, Entity> result = new HashMap<>();

        for (ExternalServiceResource resource : resources) {
            Entity entity = map(resource, externalService);
            result.put(resource, entity);
        }
        deleteStaleEntities(resources, externalService);

        return result;
    }

    private void deleteStaleEntities(Collection<ExternalServiceResource> resources, ExternalService externalService) {
        Collection<URI> resourceUris = resources.stream().map(ExternalServiceResource::getUri).collect(toSet());

        List<DiscoverableEntity> entitiesToBeRemoved = externalService.getAllOwnedEntities().stream()
                .filter(discoverable -> discoverable.getSourceUri() != null)
                .filter(discoverable -> !resourceUris.contains(discoverable.getSourceUri()))
                .collect(toList());
        genericDao.removeAndClear(entitiesToBeRemoved);
    }

    private Entity map(ExternalServiceResource resource, ExternalService service) {
        return mapperFinder.find(resource)
                .flatMap(mapper -> mapWithMapper(resource, service, mapper))
                .orElse(null);
    }

    private Optional<Entity> mapWithMapper(ExternalServiceResource resource, ExternalService service, Mapper mapper) {
        try {
            return matchResourceByServiceType(resource, service, mapper.getTargetClass())
                    .map(entity -> {
                        mapper.map(resource, entity);
                        return entity;
                    });
        } catch (IllegalStateException e) {
            logger.e("Problem while matching resource: '{}'\n{}", resource.getUri(), e.getMessage());
            return Optional.empty();
        }
    }

    private Optional<Entity> matchResourceByServiceType(ExternalServiceResource resource, ExternalService service, Class targetClass) {
        return ofNullable(
            LUI.equals(service.getServiceType())
                ? entityObtainer.obtain(service, resource)
                : externalServiceDao.findOrCreateEntity(service, resource.getUri(), targetClass)
        );
    }
}
