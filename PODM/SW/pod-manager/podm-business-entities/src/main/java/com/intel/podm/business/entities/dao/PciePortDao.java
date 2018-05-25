/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.Port.GET_PORTS_BY_PCIE_CONNECTION_ID;
import static com.intel.podm.common.types.EntityRole.INITIATOR;
import static java.util.Collections.emptySet;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class PciePortDao extends Dao<Port> {
    @Transactional(MANDATORY)
    public Set<Port> getUpstreamPortsByCableIds(List<String> pcieConnectionIds) {
        if (pcieConnectionIds.isEmpty()) {
            return emptySet();
        }

        Set<Port> ports = new HashSet<>();
        for (String pcieConnectionId : pcieConnectionIds) {
            TypedQuery<Port> query = entityManager.createNamedQuery(GET_PORTS_BY_PCIE_CONNECTION_ID, Port.class);
            query.setParameter("pcieConnectionId", pcieConnectionId);
            ports.addAll(query.getResultList());
        }

        return ports;
    }

    @Transactional(MANDATORY)
    public Collection<Port> getUpstreamPortsByDiscoverableEntity(DiscoverableEntity entity) {
        return entity.getEntityConnections().stream()
            .filter(Objects::nonNull)
            .map(connectedEntity -> connectedEntity.getEndpoint().getZone())
            .filter(Objects::nonNull)
            .map(Zone::getEndpoints)
            .flatMap(Collection::stream)
            .map(Endpoint::getConnectedEntities)
            .flatMap(Collection::stream)
            .filter(connectedEntity -> Objects.equals(INITIATOR, connectedEntity.getEntityRole()))
            .map(ConnectedEntity::getEndpoint)
            .map(Endpoint::getPorts)
            .flatMap(Collection::stream)
            .collect(toSet());
    }
}
