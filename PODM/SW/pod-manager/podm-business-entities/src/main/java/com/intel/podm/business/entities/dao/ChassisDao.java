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

import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.ApplicationScoped;
import javax.persistence.TypedQuery;
import javax.transaction.Transactional;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.entities.redfish.Chassis.GET_CHASSIS_BY_TYPE;
import static com.intel.podm.business.entities.redfish.Chassis.GET_CHASSIS_IDS_FROM_PRIMARY_DATA_SOURCE;
import static com.intel.podm.business.entities.redfish.Chassis.GET_CHASSIS_MULTI_SOURCE;
import static com.intel.podm.common.types.ChassisType.SLED;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@ApplicationScoped
public class ChassisDao extends Dao<Chassis> {
    @Transactional(MANDATORY)
    public List<Chassis> getAllByChassisType(ChassisType chassisType) {
        TypedQuery<Chassis> query = entityManager.createNamedQuery(GET_CHASSIS_BY_TYPE, Chassis.class);
        query.setParameter("chassisType", chassisType);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Id> findAllChassisFromPrimaryDataSource() {
        return entityManager.createNamedQuery(GET_CHASSIS_IDS_FROM_PRIMARY_DATA_SOURCE, Id.class).getResultList();
    }

    @Transactional(MANDATORY)
    public List<Chassis> findComplementaryChassis(Chassis chassis) {
        if (!Objects.equals(SLED, chassis.getChassisType())) {
            return emptyList();
        }

        Set<ComputerSystem> computerSystems = chassis.getComputerSystems();
        if (computerSystems.isEmpty()) {
            return emptyList();
        }
        Set<UUID> ownedComputerSystemUuids = computerSystems.stream().map(ComputerSystem::getUuid).collect(toSet());

        TypedQuery<Chassis> query = entityManager.createNamedQuery(GET_CHASSIS_MULTI_SOURCE, Chassis.class);
        query.setParameter("isComplementary", true);
        query.setParameter("type", SLED);
        query.setParameter("uuids", ownedComputerSystemUuids);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Chassis> getChassis(ChassisType chassisType, ExternalService externalService) {
        TypedQuery<Chassis> query = entityManager.createNamedQuery(Chassis.GET_CHASSIS_BY_TYPE_AND_SERVICE, Chassis.class);
        query.setParameter("chassisType", chassisType);
        query.setParameter("externalService", externalService);
        return query.getResultList();
    }

    @Transactional(MANDATORY)
    public List<Chassis> getChassis(ChassisType chassisType, String locationId) {
        return entityManager.createNamedQuery(Chassis.GET_CHASSIS_BY_TYPE_AND_LOCATION, Chassis.class)
            .setParameter("chassisType", chassisType)
            .setParameter("locationId", locationId)
            .getResultList();
    }
}
