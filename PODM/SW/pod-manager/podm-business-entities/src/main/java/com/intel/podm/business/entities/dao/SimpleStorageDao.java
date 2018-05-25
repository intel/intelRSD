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

import com.intel.podm.business.entities.redfish.SimpleStorage;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.entities.redfish.SimpleStorage.GET_SIMPLE_STORAGE_MULTI_SOURCE;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class SimpleStorageDao extends Dao<SimpleStorage> {
    @Transactional(MANDATORY)
    public Optional<SimpleStorage> findPrimarySimpleStorage(SimpleStorage complementarySimpleStorage) {
        TypedQuery<SimpleStorage> query = entityManager.createNamedQuery(GET_SIMPLE_STORAGE_MULTI_SOURCE, SimpleStorage.class);
        query.setParameter("multiSourceDiscriminator", complementarySimpleStorage.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", false);
        return ofNullable(singleOrNull(query.getResultList()));
    }

    @Transactional(MANDATORY)
    public List<SimpleStorage> findComplementarySimpleStorages(SimpleStorage simpleStorage) {
        TypedQuery<SimpleStorage> query = entityManager.createNamedQuery(GET_SIMPLE_STORAGE_MULTI_SOURCE, SimpleStorage.class);
        query.setParameter("multiSourceDiscriminator", simpleStorage.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", true);
        return query.getResultList();
    }
}
