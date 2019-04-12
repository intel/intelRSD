/*
 * Copyright (c) 2017-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.types.Protocol;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.UUID;

import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINT_MATCHING_UUID;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class EndpointDao extends Dao<Endpoint> {

    @Transactional(MANDATORY)
    public List<Endpoint> findEndpointMatchingUuid(UUID uuid) {
        TypedQuery<Endpoint> query = entityManager.createNamedQuery(GET_ENDPOINT_MATCHING_UUID, Endpoint.class);
        query.setParameter("uuid", uuid);

        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Endpoint> findEndpointsWithNullUsername(Protocol protocol) {
        TypedQuery<Endpoint> query = entityManager.createNamedQuery(GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL, Endpoint.class);
        query.setParameter("protocol", protocol);

        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Endpoint> findEndpointsWithAssociatedComputerSystem(Protocol protocol) {
        TypedQuery<Endpoint> query = entityManager.createNamedQuery(GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL,
            Endpoint.class);
        query.setParameter("protocol", protocol);

        return query.getResultList();
    }
}
