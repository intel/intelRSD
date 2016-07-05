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
import com.tinkerpop.blueprints.Vertex;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;

import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class GenericDao {
    @Inject
    private DomainObjectRepository repository;

    public <T extends DomainObject> T create(Class<T> domainObjectClass) {
        return repository.create(domainObjectClass);
    }

    public <T extends DomainObject> Optional<T> tryFind(Class<T> domainObjectClass, Id id) {
        return repository.tryGet(domainObjectClass, id);
    }

    public <T extends DomainObject> Optional<T> tryFind(Vertex vertex) {
        return repository.tryGet(vertex);
    }

    public <T extends DomainObject> T find(Class<T> domainObjectClass, Id id) {
        return repository.get(domainObjectClass, id);
    }

    public <T extends DomainObject> List<T> findAll(Class<T> domainObjectClass) {
        return repository.getAll(domainObjectClass);
    }

    public <T extends DomainObject> void remove(T domainObject) {
        repository.delete(domainObject);
    }
}
