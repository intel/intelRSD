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
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import org.springframework.stereotype.Component;

import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.rsd.collections.IterableHelper.singleOrNull;
import static com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId.oDataIdFromUri;
import static com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity.GET_DISCOVERABLE_ENTITY_BY_URI;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Component
public class DiscoverableEntityDao extends Dao<DiscoverableEntity> {

    public DiscoverableEntity findEntityOrNull(URI uri) {
        TypedQuery<DiscoverableEntity> query = entityManager.createNamedQuery(GET_DISCOVERABLE_ENTITY_BY_URI, DiscoverableEntity.class);
        ODataId resourceODataId = oDataIdFromUri(uri);
        query.setParameter("uri", resourceODataId);
        return singleOrNull(query.getResultList());
    }

    @Transactional(MANDATORY)
    public DiscoverableEntity findOrCreateEntity(URI uri, Class clazz) {
        DiscoverableEntity entityOrNull = findEntityOrNull(uri);
        if (entityOrNull == null) {
            ODataId resourceODataId = oDataIdFromUri(uri);
            return createEntity(resourceODataId, clazz);
        }
        return entityOrNull;
    }

    @Transactional(MANDATORY)
    public <T extends DiscoverableEntity> T createEntity(ODataId oDataId, Class<T> clazz) {
        T entity = create(clazz);
        entity.setUri(oDataId);
        return entity;
    }

}
