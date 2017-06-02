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

import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.EntityType;
import com.intel.podm.common.types.PortType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.stream.Stream;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class DriveDao extends Dao<Drive> {
    @Inject
    private Logger logger;

    @Inject
    private PciePortDao pciePortDao;

    public Set<Drive> getAchievablePcieDrives(ComputerSystem computerSystem) {
        List<String> pcieConnectionIds = computerSystem.getPcieConnectionId();

        return pciePortDao.getUpstreamPortsByCableIds(pcieConnectionIds).stream()
            .map(this::getAchievablePcieDrives)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    public Set<Drive> getAchievablePcieDrives(Port upstreamPort) {
        return Stream.of(upstreamPort)
            .map(Port::getSwitch)
            .map(Switch::getPorts)
            .flatMap(Collection::stream)
            .filter(pciePort -> pciePort.getPortType().equals(PortType.DOWNSTREAM_PORT))
            .filter(pciePort -> pciePort.getEndpoints() != null)
            .map(Port::getEndpoints)
            .flatMap(Collection::stream)
            .map(Endpoint::getConnectedEntities)
            .flatMap(Collection::stream)
            .filter(connectedEntity -> connectedEntity.getEntityType() == EntityType.DRIVE)
            .map(ConnectedEntity::getEntityLink)
            .filter(Objects::nonNull)
            .filter(entity -> entity instanceof Drive)
            .map(Drive.class::cast)
            .filter(pcieDrive -> !pcieDrive.getMetadata().isAllocated())
            .peek(pcieDrive -> logger.t("Drive preselected after applying 'is already allocated' filter: {}", pcieDrive))
            .filter(DiscoverableEntity::canBeAllocated)
            .peek(pcieDrive -> logger.t("Drive preselected after applying 'can be allocated' filter: {}", pcieDrive))
            .collect(toSet());
    }
}
