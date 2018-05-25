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

import com.intel.podm.business.entities.redfish.Memory;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.entities.redfish.Memory.GET_MEMORY_MULTI_SOURCE;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Optional.ofNullable;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class MemoryDao extends Dao<Memory> {
    @Transactional(MANDATORY)
    public List<Memory> findComplementaryMemory(Memory memory) {
        TypedQuery<Memory> query = entityManager.createNamedQuery(GET_MEMORY_MULTI_SOURCE, Memory.class);
        query.setParameter("multiSourceDiscriminator", memory.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", true);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public Optional<Memory> findPrimaryMemory(Memory complementaryMemory) {
        TypedQuery<Memory> query = entityManager.createNamedQuery(GET_MEMORY_MULTI_SOURCE, Memory.class);
        query.setParameter("multiSourceDiscriminator", complementaryMemory.getMultiSourceDiscriminator());
        query.setParameter("isComplementary", false);
        return ofNullable(singleOrNull(query.getResultList()));
    }
}
