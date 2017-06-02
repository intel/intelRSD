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


import com.intel.podm.business.entities.NonUniqueResultException;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.Iterator;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;

@Dependent
public class Dao<T extends Entity> {
    protected Class<T> entityClass;

    @PersistenceContext
    protected EntityManager entityManager;

    @Inject
    private EntityRepository repository;

    @SuppressWarnings("unchecked")
    public Dao() {
        if (getClass().equals(Dao.class)) {
            entityClass = (Class<T>) Entity.class;
        } else {
            Type type = getClass().getGenericSuperclass();
            while (!(type instanceof ParameterizedType) || ((ParameterizedType) type).getRawType() != Dao.class) {
                if (type instanceof ParameterizedType) {
                    type = ((Class<?>) ((ParameterizedType) type).getRawType()).getGenericSuperclass();
                } else {
                    type = ((Class<?>) type).getGenericSuperclass();
                }
            }
            entityClass = (Class<T>) ((ParameterizedType) type).getActualTypeArguments()[0];
        }
    }

    public T create() {
        return repository.create(entityClass);
    }

    public Optional<T> tryFind(Id id) {
        return repository.tryFind(entityClass, id);
    }

    public T find(Id id) {
        return repository.find(entityClass, id);
    }

    public List<T> findAll() {
        return repository.findAll(entityClass);
    }

    public void remove(T entity) {
        repository.remove(entity);
    }


    public <X extends Entity> X create(Class<X> entityClass) {
        return repository.create(entityClass);
    }

    static <T> T singleEntityOrNull(Iterable<T> queryResult) throws NonUniqueResultException {
        requiresNonNull(queryResult, "queryResult");
        Iterator<T> iterator = queryResult.iterator();

        if (!queryResult.iterator().hasNext()) {
            return null;
        } else {
            T element = iterator.next();
            if (iterator.hasNext()) {
                throw new NonUniqueResultException("Couldn't find single result.!");
            }
            return element;
        }
    }
}
