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
import com.intel.podm.common.types.Protocol;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.Endpoint.GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_SERVICE_UUID_AND_PROTOCOL;
import static com.intel.podm.business.entities.redfish.Endpoint.GET_ENDPOINTS_WITH_NULL_USERNAME_BY_SERVICE_UUID_AND_PROTOCOL;
import static com.intel.podm.business.entities.redfish.Endpoint.GET_ENDPOINT_MATCHING_UUID;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class EndpointDao extends Dao<Endpoint> {

    @Transactional(MANDATORY)
    public List<Endpoint> findEndpointMatchingUuid(UUID uuid) {
        TypedQuery<Endpoint> query = entityManager.createNamedQuery(GET_ENDPOINT_MATCHING_UUID, Endpoint.class);
        query.setParameter("uuid", uuid);

        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Endpoint> findEndpointsWithNullUsername(UUID serviceUuid, Protocol protocol) {
        TypedQuery<Endpoint> query = entityManager.createNamedQuery(GET_ENDPOINTS_WITH_NULL_USERNAME_BY_SERVICE_UUID_AND_PROTOCOL, Endpoint.class);
        query.setParameter("serviceUuid", serviceUuid);
        query.setParameter("protocol", protocol);

        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Endpoint> findEndpointsWithAssociatedComputerSystem(UUID serviceUuid, Protocol protocol) {
        TypedQuery<Endpoint> query = entityManager.createNamedQuery(GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_SERVICE_UUID_AND_PROTOCOL,
            Endpoint.class);
        query.setParameter("serviceUuid", serviceUuid);
        query.setParameter("protocol", protocol);

        return query.getResultList();
    }
}
