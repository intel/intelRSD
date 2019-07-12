/*
 * Copyright (c) 2016-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.List;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.EntityRole.INITIATOR;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toSet;

@Slf4j
@Component
public class DriveDao extends Dao<Drive> {
    private final PciePortDao pciePortDao;

    @Autowired
    public DriveDao(PciePortDao pciePortDao) {
        this.pciePortDao = pciePortDao;
    }

    public Set<Drive> getAchievablePcieDrives(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "ComputerSystem can not be null.");

        List<String> pcieConnectionIds = computerSystem.getPcieConnectionIds();

        return pciePortDao.getUpstreamPortsByCableIds(pcieConnectionIds).stream()
            .map(this::getAchievablePcieDrives)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    public Endpoint getConnectedInitiatorEndpoint(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "ComputerSystem can not be null.");

        List<String> pcieConnectionIds = computerSystem.getPcieConnectionIds();

        return pciePortDao.getUpstreamPortsByCableIds(pcieConnectionIds).stream()
            .map(Port::getEndpoints)
            .flatMap(Collection::stream)
            .filter(endpoint -> endpoint.hasRole(INITIATOR))
            .findFirst()
            .orElse(null);
    }

    public Set<Drive> getAchievablePcieDrives(Port upstreamPort) {

        return tryGetDrivesThatBelongToTheSameSwitch(upstreamPort)
            .stream()
            .filter(pcieDrive -> !pcieDrive.isAllocated())
            .peek(pcieDrive -> log.trace("Drive preselected after applying 'is already allocated' filter: {}", pcieDrive))
            .filter(Drive::isAvailable)
            .peek(pcieDrive -> log.trace("Drive preselected after applying 'can be allocated' filter: {}", pcieDrive))
            .collect(toSet());
    }

    private Set<Drive> tryGetDrivesThatBelongToTheSameSwitch(Port upstreamPort) {
        return upstreamPort.getSwitch().getChassis().stream()
            .flatMap(chassis -> chassis.getAllDrivesUnderneath().stream())
            .collect(toSet());
    }
}
