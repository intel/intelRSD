/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.Volume;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.redfish.base.ConnectedEntity.GET_CONNECTED_ENTITY_BY_ENTITY_LINK_AND_ENDPOINT;
import static com.intel.podm.common.utils.IterableHelper.single;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class ConnectedEntityDao extends Dao<Endpoint> {

    @Transactional(MANDATORY)
    public ConnectedEntity getConnectedEntity(Endpoint endpoint, Volume volume) {
        TypedQuery<ConnectedEntity> query = entityManager.createNamedQuery(GET_CONNECTED_ENTITY_BY_ENTITY_LINK_AND_ENDPOINT, ConnectedEntity.class);
        query.setParameter("endpoint", endpoint);
        query.setParameter("volume", volume);

        return single(query.getResultList());
    }
}
