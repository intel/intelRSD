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

import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;
import java.util.Optional;
import java.util.function.Predicate;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class GenericDao {
    @Inject
    private EntityRepository repository;

    public <T extends Entity> T create(Class<T> entityClass) {
        return repository.create(entityClass);
    }

    public <T extends Entity> Optional<T> tryFind(Class<T> entityClass, Id id) {
        return repository.tryFind(entityClass, id);
    }

    public <T extends Entity> T find(Class<T> entityClass, Id id) {
        return repository.find(entityClass, id);
    }

    public <T extends Entity> List<T> findAll(Class<T> entityClass) {
        return repository.findAll(entityClass);
    }

    public <T extends Entity> void remove(T entity) {
        repository.remove(entity);
    }

    public <T extends Entity> void removeAndClear(Collection<T> entities) {
        repository.removeAndClear(entities);
    }

    public <T extends Entity> void removeAndClear(Collection<T> entities, Predicate<T> predicate) {
        repository.removeAndClear(entities, predicate);
    }
}
