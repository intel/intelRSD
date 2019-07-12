/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.dao;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.EntityNotFoundException;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Optional;
import java.util.function.Predicate;

import static com.intel.rsd.collections.IterableHelper.singleOrNull;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
class EntityRepository {
    @PersistenceContext
    protected EntityManager entityManager;

    @Transactional(MANDATORY)
    public <T extends Entity> void persist(T entity) {
        requiresNonNull(entity, "entity");
        entityManager.persist(entity);
    }

    @Transactional(MANDATORY)
    public <T extends Entity> T create(Class<T> entityClass) {
        requiresNonNull(entityClass, "entityClass");

        T entity;
        try {
            entity = entityClass.newInstance();
        } catch (InstantiationException | IllegalAccessException e) {
            throw new IllegalStateException("Couldn't instantiate entity of type: " + entityClass);
        }

        entityManager.persist(entity);
        return entity;
    }

    //TODO: pzak - Entity does not have uri - maybe it should be moved from DiscoverableEntity?
    public <T extends Entity> Optional<T> tryFind(Class<T> entityClass, ODataId uri) {
        TypedQuery<T> query = entityManager.createQuery("SELECT e FROM " + entityClass.getSimpleName() + " e WHERE e.uri = :uri", entityClass);
        query.setParameter("uri", uri);
        return ofNullable(singleOrNull(query.getResultList()));
    }

    //TODO: pzak - Entity does not have uri - maybe it should be moved from DiscoverableEntity?
    public <T extends Entity> T find(Class<T> entityClass, ODataId uri) {
        return tryFind(entityClass, uri).orElseThrow(
            () -> new EntityNotFoundException(entityClass, uri)
        );
    }

    public <T extends Entity> List<T> findAll(Class<T> entityClass) {
        TypedQuery<T> query =
            entityManager.createQuery("SELECT e FROM " + entityClass.getSimpleName() + " e ORDER BY e.id", entityClass);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void remove(T entity) {
        if (entity != null) {
            if (entityManager.contains(entity)) {
                entityManager.remove(entity);
            } else {
                log.error("Found a detached entity: {}", entity);
            }
        }
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void removeAndClear(Collection<T> entities) {
        removeAndClear(entities, x -> true);
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void removeAndClear(Collection<T> entities, Predicate<T> predicate) {
        if (entities != null) {
            //TODO: RSASW-8093
            Iterator<T> iterator = entities.iterator();
            while (iterator.hasNext()) {
                T entity = iterator.next();
                if (predicate.test(entity)) {
                    iterator.remove();
                    remove(entity);
                    iterator = entities.iterator();
                }
            }
        }
    }
}
