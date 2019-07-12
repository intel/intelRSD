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
import com.intel.rsd.nodecomposer.persistence.redfish.base.Entity;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;
import java.util.Optional;
import java.util.function.Predicate;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Component
public class GenericDao {

    @Autowired
    private EntityRepository repository;

    @Transactional(MANDATORY)
    public <T extends Entity> T create(Class<T> entityClass) {
        return repository.create(entityClass);
    }

    public <T extends Entity> Optional<T> tryFind(Class<T> entityClass, ODataId uri) {
        return repository.tryFind(entityClass, uri);
    }

    public <T extends Entity> T find(Class<T> entityClass, ODataId uri) {
        return repository.find(entityClass, uri);
    }

    public <T extends Entity> List<T> findAll(Class<T> entityClass) {
        return repository.findAll(entityClass);
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void remove(T entity) {
        repository.remove(entity);
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void removeAndClear(Collection<T> entities) {
        repository.removeAndClear(entities);
    }

    @Transactional(MANDATORY)
    public <T extends Entity> void removeAndClear(Collection<T> entities, Predicate<T> predicate) {
        repository.removeAndClear(entities, predicate);
    }
}
