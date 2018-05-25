/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.strategy.matcher;

import com.intel.podm.business.entities.dao.DriveDao;
import com.intel.podm.business.entities.dao.PciePortDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.base.ConnectedEntity;
import com.intel.podm.business.entities.redfish.base.LocalStorage;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.common.utils.Converters.convertBytesToGib;
import static com.intel.podm.common.utils.IterableHelper.optionalSingle;
import static java.lang.Boolean.FALSE;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;

@Dependent
public class LocalStorageCollector {
    @Inject
    private PciePortDao pciePortDao;

    @Inject
    private DriveDao pcieDriveDao;

    private static LocalDrive toLocalDrive(Drive drive) {
        LocalDrive driveLocalStorage = new LocalDrive();
        driveLocalStorage.setId(drive.getId());
        driveLocalStorage.setParent(drive.getChassis());
        driveLocalStorage.setCapacityGib(convertBytesToGib(drive.getCapacityBytes()));
        driveLocalStorage.setSerialNumber(drive.getSerialNumber());
        driveLocalStorage.setType(drive.getMediaType());
        driveLocalStorage.setDrive(drive);
        driveLocalStorage.setProtocol(drive.getProtocol());
        driveLocalStorage.setRpm(drive.getRotationSpeedRpm());
        return driveLocalStorage;
    }

    public Set<LocalStorage> getStorageUnderComputerSystem(ComputerSystem computerSystem) {
        Set<LocalStorage> localStorage = getSimpleStorage(computerSystem);
        localStorage.addAll(getPcieStorage(computerSystem));
        localStorage.addAll(getLocalDrivesFromStorage(computerSystem));
        return localStorage;
    }

    private List<LocalDrive> getLocalDrivesFromStorage(ComputerSystem computerSystem) {
        if (computerSystem == null) {
            return emptyList();
        }

        return computerSystem.getStorages().stream()
            .map(Storage::getDrives)
            .flatMap(Collection::stream)
            .map(LocalStorageCollector::toLocalDrive)
            .collect(toList());
    }

    private Set<LocalStorage> getSimpleStorage(ComputerSystem computerSystem) {
        return computerSystem.getSimpleStorages().stream()
            .map(SimpleStorage::getDevices)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    public Set<PcieLocalStorage> getPcieStorage(ComputerSystem computerSystem) {
        Set<Port> upstreamPorts = getUpstreamPortsAchievableFromComputerSystem(computerSystem);
        Set<PcieLocalStorage> pcieLocalStorages = new HashSet<>();

        for (Port upstreamPort : upstreamPorts) {
            Set<Drive> achievablePcieDrives = pcieDriveDao.getAchievablePcieDrives(upstreamPort);
            pcieLocalStorages.addAll(achievablePcieDrives.stream()
                .map(drive -> convertToPcieLocalStorage(drive, upstreamPort))
                .collect(toSet()));
        }

        return pcieLocalStorages;
    }

    private Set<Port> getUpstreamPortsAchievableFromComputerSystem(ComputerSystem computerSystem) {
        List<String> pcieConnectionIds = computerSystem.getPcieConnectionIds();

        return pciePortDao.getUpstreamPortsByCableIds(pcieConnectionIds);
    }

    private PcieLocalStorage convertToPcieLocalStorage(Drive drive, Port upstreamPort) {
        PcieLocalStorage pcieLocalStorage = new PcieLocalStorage();
        pcieLocalStorage.setId(drive.getId());
        pcieLocalStorage.setParent(drive.getChassis());
        pcieLocalStorage.setCapacityGib(convertBytesToGib(drive.getCapacityBytes()));
        pcieLocalStorage.setType(drive.getMediaType());
        pcieLocalStorage.setRpm(null);
        pcieLocalStorage.setProtocol(NVME);
        pcieLocalStorage.setDrive(drive);
        pcieLocalStorage.setEndpoint(optionalSingle(drive.getEntityConnections()).map(ConnectedEntity::getEndpoint).orElse(null));

        Endpoint upstreamEndpointInUpstreamPort = upstreamPort.getEndpoints().stream()
            .filter(endpoint -> endpoint.getZone() != null)
            .findFirst().orElse(null);
        pcieLocalStorage.setZone(upstreamEndpointInUpstreamPort != null ? upstreamEndpointInUpstreamPort.getZone() : null);

        pcieLocalStorage.setPcieDeviceFunction(drive.getPcieDeviceFunction());
        pcieLocalStorage.setSerialNumber(drive.getSerialNumber());
        pcieLocalStorage.setNeedsToBeExplicitlySelected(Objects.equals(FALSE, drive.getDriveErased()));
        return pcieLocalStorage;
    }

    public Set<Drive> getLocalDrivesFromChassis(Set<Chassis> chassis) {
        return chassis.stream()
            .map(Chassis::getAllDrivesUnderneath)
            .flatMap(Collection::stream)
            .collect(toSet());
    }
}
