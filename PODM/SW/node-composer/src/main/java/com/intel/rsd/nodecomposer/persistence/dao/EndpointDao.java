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

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.NonNull;
import lombok.val;
import org.springframework.stereotype.Component;

import java.util.List;

import static com.intel.rsd.collections.IterableHelper.singleOrNull;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_INITIATOR_ENDPOINT_BY_FABRIC_AND_SYSTEM;
import static com.intel.rsd.nodecomposer.persistence.redfish.Endpoint.GET_INITIATOR_ENDPOINT_BY_STORAGE_SERVICE_AND_SYSTEM;
import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;

@Component
public class EndpointDao extends Dao<Endpoint> {

    public Endpoint findInitiatorEndpointBySystemAndFabric(@NonNull ODataId computerSystemOdataId, @NonNull ODataId fabricOdataId) {
        val query = entityManager.createNamedQuery(GET_INITIATOR_ENDPOINT_BY_FABRIC_AND_SYSTEM, Endpoint.class)
            .setParameter("role", INITIATOR)
            .setParameter("system", computerSystemOdataId)
            .setParameter("fabric", fabricOdataId);

        return singleOrNull(query.getResultList());
    }

    public Endpoint findInitiatorEndpointBySystemAndStorageService(@NonNull ODataId computerSystemOdataId, @NonNull ODataId storageServiceOdataId) {
        val query = entityManager.createNamedQuery(GET_INITIATOR_ENDPOINT_BY_STORAGE_SERVICE_AND_SYSTEM, Endpoint.class)
            .setParameter("role", INITIATOR)
            .setParameter("system", computerSystemOdataId)
            .setParameter("ss", storageServiceOdataId);

        return singleOrNull(query.getResultList());
    }

    public List<Endpoint> findEndpointsWithNullUsername(Protocol protocol) {
        val query = entityManager.createNamedQuery(GET_ENDPOINTS_WITH_NULL_USERNAME_BY_PROTOCOL, Endpoint.class).setParameter("protocol", protocol);

        return query.getResultList();
    }

    public List<Endpoint> findEndpointsWithAssociatedComputerSystem(Protocol protocol) {
        val query = entityManager.createNamedQuery(GET_ENDPOINTS_ASSOCIATED_WITH_COMPUTER_SYSTEM_BY_PROTOCOL, Endpoint.class)
            .setParameter("protocol", protocol);

        return query.getResultList();
    }
}
