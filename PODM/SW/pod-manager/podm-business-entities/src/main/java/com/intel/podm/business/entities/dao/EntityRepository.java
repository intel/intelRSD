/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.Optional;
import java.util.function.Predicate;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
class EntityRepository {
    @PersistenceContext
    protected EntityManager entityManager;

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

    @Transactional(MANDATORY)
    public <T extends Entity> Optional<T> tryFind(Class<T> entityClass, Id entityId) {
        TypedQuery<T> query = entityManager
            .createQuery("SELECT e FROM " + entityClass.getSimpleName() + " e WHERE e.entityId = :entityId", entityClass);
        query.setParameter("entityId", entityId);
        return ofNullable(singleOrNull(query.getResultList()));
    }

    @Transactional(MANDATORY)
    public <T extends Entity> T find(Class<T> entityClass, Id entityId) {
        return tryFind(entityClass, entityId).orElseThrow(
            () -> new EntityNotFoundException(entityClass, entityId)
        );
    }

    @Transactional(MANDATORY)
    public <T extends Entity> List<T> findAll(Class<T> entityClass) {
        TypedQuery<T> query =
            entityManager.createQuery("SELECT e FROM " + entityClass.getSimpleName() + " e ORDER BY e.entityId", entityClass);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void remove(T entity) {
        if (entity != null) {
            entityManager.remove(entity);
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
