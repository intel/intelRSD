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

package com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher;

import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.persistence.dao.DriveDao;
import com.intel.rsd.nodecomposer.persistence.dao.PciePortDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Chassis;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.SimpleStorage;
import com.intel.rsd.nodecomposer.persistence.redfish.Storage;
import com.intel.rsd.nodecomposer.persistence.redfish.base.LocalStorage;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.composition.allocation.strategy.ODataIdPossessorUtils.getChassisODataIds;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.utils.Converters.convertBytesToGib;
import static java.lang.Boolean.FALSE;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class LocalStorageCollector {
    private final PciePortDao pciePortDao;
    private final DriveDao pcieDriveDao;
    private final ChassisCollector chassisCollector;

    @Autowired
    public LocalStorageCollector(PciePortDao pciePortDao, DriveDao pcieDriveDao, ChassisCollector chassisCollector) {
        this.pciePortDao = pciePortDao;
        this.pcieDriveDao = pcieDriveDao;
        this.chassisCollector = chassisCollector;
    }

    public Set<Drive> getLocalDrivesFromChassisODataId(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        return getLocalDrivesFromChassis(
            chassisCollector.getChassisFromODataIds(getChassisODataIds(requestedNode.getLocalDrives()))
        );
    }

    public Set<LocalStorage> getStorageUnderComputerSystem(ComputerSystem computerSystem) {
        Set<LocalStorage> localStorage = getSimpleStorage(computerSystem);
        localStorage.addAll(getPcieStorage(computerSystem));
        localStorage.addAll(getLocalDrivesFromStorage(computerSystem));
        return localStorage;
    }

    private Set<PcieLocalStorage> getPcieStorage(ComputerSystem computerSystem) {
        Set<Port> upstreamPorts = getUpstreamPortsAchievableFromComputerSystem(computerSystem);
        Set<PcieLocalStorage> pcieLocalStorages = new HashSet<>();

        for (Port upstreamPort : upstreamPorts) {
            Set<Drive> achievablePcieDrives = pcieDriveDao.getAchievablePcieDrives(upstreamPort);
            pcieLocalStorages.addAll(achievablePcieDrives.stream()
                .filter(Drive::isAchievable)
                .map(this::convertToPcieLocalStorage)
                .collect(toSet()));
        }

        return pcieLocalStorages;
    }

    private static LocalDrive toLocalDrive(Drive drive) {
        LocalDrive driveLocalStorage = new LocalDrive();
        driveLocalStorage.setParent(drive.getChassis());
        driveLocalStorage.setCapacityGib(convertBytesToGib(drive.getCapacityBytes()));
        driveLocalStorage.setSerialNumber(drive.getSerialNumber());
        driveLocalStorage.setType(drive.getMediaType());
        driveLocalStorage.setDrive(drive);
        driveLocalStorage.setProtocol(drive.getProtocol());
        driveLocalStorage.setRpm(drive.getRotationSpeedRpm());
        driveLocalStorage.setUri(drive.getUri());
        return driveLocalStorage;
    }

    private Set<Drive> getLocalDrivesFromChassis(Set<Chassis> chassis) {
        return chassis.stream()
            .map(Chassis::getAllDrivesUnderneath)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    private List<LocalDrive> getLocalDrivesFromStorage(ComputerSystem computerSystem) {
        if (computerSystem == null) {
            return emptyList();
        }

        return computerSystem.getStorages().stream()
            .map(Storage::getDrives)
            .flatMap(Collection::stream)
            .filter(Drive::isAchievable)
            .map(LocalStorageCollector::toLocalDrive)
            .collect(toList());
    }

    private Set<LocalStorage> getSimpleStorage(ComputerSystem computerSystem) {
        return computerSystem.getSimpleStorages().stream()
            .map(SimpleStorage::getDevices)
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    private Set<Port> getUpstreamPortsAchievableFromComputerSystem(ComputerSystem computerSystem) {
        List<String> pcieConnectionIds = computerSystem.getPcieConnectionIds();

        return pciePortDao.getUpstreamPortsByCableIds(pcieConnectionIds);
    }

    private PcieLocalStorage convertToPcieLocalStorage(Drive drive) {
        PcieLocalStorage pcieLocalStorage = new PcieLocalStorage();
        pcieLocalStorage.setUri(drive.getUri());
        pcieLocalStorage.setParent(drive.getChassis());
        pcieLocalStorage.setCapacityGib(convertBytesToGib(drive.getCapacityBytes()));
        pcieLocalStorage.setType(drive.getMediaType());
        pcieLocalStorage.setRpm(null);
        pcieLocalStorage.setProtocol(NVME);
        pcieLocalStorage.setDrive(drive);
        pcieLocalStorage.setPcieDeviceFunction(drive.getPcieDeviceFunction());
        pcieLocalStorage.setSerialNumber(drive.getSerialNumber());
        pcieLocalStorage.setNeedsToBeExplicitlySelected(Objects.equals(FALSE, drive.getDriveErased()));
        return pcieLocalStorage;
    }
}
