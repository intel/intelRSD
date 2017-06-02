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

import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;

import javax.enterprise.context.Dependent;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;

import static com.intel.podm.business.entities.redfish.base.DiscoverableEntity.GET_DISCOVERABLE_ENTITY_BY_EXTERNAL_SERVICE_AND_SOURCE_URI;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class DiscoverableEntityDao extends Dao<DiscoverableEntity> {
    public List<DiscoverableEntity> getByExternalServiceAndSourceUri(ExternalService externalService, URI sourceUri) {
        TypedQuery<DiscoverableEntity> query =
            entityManager.createNamedQuery(GET_DISCOVERABLE_ENTITY_BY_EXTERNAL_SERVICE_AND_SOURCE_URI, DiscoverableEntity.class);
        query.setParameter("externalService", externalService);
        query.setParameter("sourceUri", sourceUri);
        return query.getResultList();
    }
}
