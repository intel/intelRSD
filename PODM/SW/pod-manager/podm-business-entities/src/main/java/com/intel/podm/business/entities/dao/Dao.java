/*
 * Copyright (c) 2015 Intel Corporation
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

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectRepository;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.util.List;
import java.util.Optional;

@Dependent
public class Dao<T extends DomainObject> {
    @Inject
    protected DomainObjectRepository repository;

    private final Class<T> domainObjectClass;

    @SuppressWarnings("unchecked")
    public Dao() {
        if (getClass().equals(Dao.class)) {
            domainObjectClass = (Class<T>) DomainObject.class;
        } else {
            Type type = getClass().getGenericSuperclass();
            while (!(type instanceof ParameterizedType) || ((ParameterizedType) type).getRawType() != Dao.class) {
                if (type instanceof ParameterizedType) {
                    type = ((Class<?>) ((ParameterizedType) type).getRawType()).getGenericSuperclass();
                } else {
                    type = ((Class<?>) type).getGenericSuperclass();
                }
            }
            domainObjectClass = (Class<T>) ((ParameterizedType) type).getActualTypeArguments()[0];
        }
    }

    public T create() {
        return repository.create(domainObjectClass);
    }

    public Optional<T> tryFind(Id domainObjectId) {
        return repository.tryGet(domainObjectClass, domainObjectId);
    }

    public T find(Id domainObjectId) {
        return repository.get(domainObjectClass, domainObjectId);
    }

    public List<T> findAll() {
        return repository.getAll(domainObjectClass);
    }

    public void remove(T domainObject) {
        repository.delete(domainObject);
    }
}
