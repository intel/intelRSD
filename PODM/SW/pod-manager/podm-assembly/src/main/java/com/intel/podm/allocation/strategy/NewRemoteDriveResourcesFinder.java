/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.allocation.strategy;

import com.intel.podm.allocation.validation.Violations;
import com.intel.podm.business.dto.redfish.RequestedMasterDrive;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;
import com.intel.podm.business.entities.DomainObjectNotFoundException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.LogicalDrive;
import com.intel.podm.business.entities.redfish.StorageService;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.math.BigDecimal;
import java.util.Optional;

@Dependent
public class NewRemoteDriveResourcesFinder {
    @Inject
    private GenericDao genericDao;

    private Optional<LogicalDrive> lvg;

    public NewRemoteDriveAllocationResources find(RequestedRemoteDrive drive) {
        NewRemoteDriveAllocationResources resources = new NewRemoteDriveAllocationResources();
        Violations violations = new Violations();

        findLvg(drive, violations);

        if (!violations.hasViolations()) {
            LogicalDrive master = getMasterDrive(drive.getMaster());
            resources.setMaster(master);
            resources.setLvg(lvg.get());
            resources.setCapacity(drive.getCapacityGib());
        }

        resources.setViolations(violations);

        return resources;
    }

    private LogicalDrive getMasterDrive(RequestedMasterDrive masterDrive) {
        try {
            return genericDao.find(LogicalDrive.class, masterDrive.getResourceContext().getId());
        } catch (DomainObjectNotFoundException e) {
            throw new IllegalStateException("Provided master drive no longer exist.");
        }
    }

    private void findLvg(RequestedRemoteDrive drive, Violations violations) {
        StorageService storageService = genericDao.find(StorageService.class, drive.getMaster().getResourceContext().getParent().getId());
        lvg = findLvgMatchingFreeSpaceRequirement(storageService, drive.getCapacityGib());

        if (!lvg.isPresent()) {
            violations.addViolation("LVG with sufficient space was not found.");
        }
    }

    private Optional<LogicalDrive> findLvgMatchingFreeSpaceRequirement(StorageService storageService, BigDecimal capacityGib) {
        return storageService.getLogicalVolumeGroups()
                .stream()
                .filter(lvg -> capacityGib.compareTo(lvg.getFreeSpaceGib()) <= 0)
                .findFirst();
    }

    static class NewRemoteDriveAllocationResources {
        LogicalDrive lvg;
        LogicalDrive master;
        Violations violations;
        BigDecimal capacity;

        public LogicalDrive getLvg() {
            return lvg;
        }

        public void setLvg(LogicalDrive lvg) {
            this.lvg = lvg;
        }

        public LogicalDrive getMaster() {
            return master;
        }

        public void setMaster(LogicalDrive master) {
            this.master = master;
        }

        public Violations getViolations() {
            return violations;
        }

        public void setViolations(Violations violations) {
            this.violations = violations;
        }

        public BigDecimal getCapacity() {
            return capacity;
        }

        public void setCapacity(BigDecimal capacity) {
            this.capacity = capacity;
        }
    }
}
