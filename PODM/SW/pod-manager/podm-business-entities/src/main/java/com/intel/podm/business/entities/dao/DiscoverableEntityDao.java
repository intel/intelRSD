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

package com.intel.podm.business.entities.dao;

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ExternalLinkDao;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.function.Function;

import static com.intel.podm.business.entities.redfish.base.DiscoverableEntity.GET_ENTITY_BY_SERVICE_AND_SOURCE_URI;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class DiscoverableEntityDao extends Dao<DiscoverableEntity> {

    @Inject
    private GenericDao genericDao;

    @Inject
    private ExternalLinkDao externalLinkDao;

    private HashMap<Class<?>, FindOrCreateStrategy> findOrCreateStrategyRegistry = new HashMap<Class<?>, FindOrCreateStrategy>() {

        {
            put(Chassis.class, new CombinedEntityCreator());
        }

        @Override
        public FindOrCreateStrategy get(Object o) {
            return super.getOrDefault(o, new RegularEntityCreator());
        }
    };

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T findByGlobalId(Id globalId, Class<T> clazz) {
        return singleOrNull(entityManager.createQuery("SELECT e FROM " + clazz.getSimpleName() + " e WHERE e.globalId = :globalId", clazz)
            .setParameter("globalId", globalId)
            .getResultList());
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> void removeWithConnectedExternalLinks(T entity) {
        genericDao.removeAndClear(entity.getExternalLinks());
        genericDao.remove(entity);
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T createEntity(ExternalService externalService, Id entityId, Class<T> clazz) {
        T entity = create(clazz);
        entity.setId(entityId);
        entity.setGlobalId(entityId);
        entity.setComplementary(externalService.isComplementaryDataSource());
        return entity;
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T findOrCreateEntity(ExternalService externalService, Id globalId, URI uri, Class<T> clazz) {
        FindOrCreateStrategy entityCreator = findOrCreateStrategyRegistry.get(clazz);
        return entityCreator.findOrCreate(externalService, globalId, uri, clazz);
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> void removeWithConnectedExternalLinks(Collection<T> discoverableEntities) {
        new HashSet<>(discoverableEntities).forEach(this::removeWithConnectedExternalLinks);
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity, S extends DiscoverableEntity>
    void removeWithConnectedExternalLinks(Collection<T> entities, Function<T, Collection<S>> subResourceProvider) {
        entities.forEach(de -> removeWithConnectedExternalLinks(subResourceProvider.apply(de)));
        removeWithConnectedExternalLinks(entities);
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T findBy(ExternalService externalService, URI sourceUri, Class<T> clazz) {
        TypedQuery<T> query = entityManager.createNamedQuery(GET_ENTITY_BY_SERVICE_AND_SOURCE_URI, clazz);
        query.setParameter("externalService", externalService);
        query.setParameter("sourceUri", sourceUri);
        return singleOrNull(query.getResultList());
    }

    interface FindOrCreateStrategy {
        <T extends DiscoverableEntity> T findOrCreate(ExternalService externalService, Id globalId, URI uri, Class<T> clazz);
    }

    class CombinedEntityCreator implements FindOrCreateStrategy {
        @Override
        public <T extends DiscoverableEntity> T findOrCreate(ExternalService externalService, Id globalId, URI uri, Class<T> clazz) {
            T discoverableEntity = findByGlobalId(globalId, clazz);
            if (discoverableEntity == null) {
                discoverableEntity = createEntity(externalService, globalId, clazz);
                externalLinkDao.create(uri, externalService, discoverableEntity);
            } else {
                externalLinkDao.createIfNotExisting(uri, externalService, discoverableEntity);
            }
            return discoverableEntity;
        }
    }

    @SuppressWarnings({"unchecked"})
    class RegularEntityCreator implements FindOrCreateStrategy {
        @Override
        public <T extends DiscoverableEntity> T findOrCreate(ExternalService externalService, Id globalId, URI uri, Class<T> clazz) {
            T discoverableEntity = (T) externalService.getOwnedEntities().stream()
                .filter(e -> e.getGlobalId().equals(globalId))
                .findFirst()
                .orElse(null);

            if (discoverableEntity == null) {
                discoverableEntity = createEntity(externalService, globalId, clazz);
                externalLinkDao.create(uri, externalService, discoverableEntity);
            }
            return discoverableEntity;
        }
    }
}
