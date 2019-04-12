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

package com.intel.rsd.nodecomposer.persistence.dao;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class Dao<T extends Entity> {
    protected Class<T> entityClass;

    @PersistenceContext
    protected EntityManager entityManager;

    @Autowired
    private EntityRepository repository;

    @SuppressWarnings("unchecked")
    public Dao() {
        if (Objects.equals(getClass(), Dao.class)) {
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

    public Optional<T> tryFind(ODataId uri) {
        return repository.tryFind(entityClass, uri);
    }

    public T find(ODataId uri) {
        return repository.find(entityClass, uri);
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

    public <X extends Entity> void persist(X entity) {
        repository.persist(entity);
    }
}
