/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.dto.StorageDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.services.aggregation.ChassisSubResourcesFinder;
import com.intel.podm.business.redfish.services.aggregation.StorageSubResourcesFinder;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.business.redfish.ContextCollections.asChassisContexts;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.toList;

@Dependent
class StorageDtoMapper extends DtoMapper<Storage, StorageDto> {
    @Inject
    private ChassisSubResourcesFinder chassisSubResourcesFinder;

    @Inject
    private StorageSubResourcesFinder storageSubResourcesFinder;

    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    StorageDtoMapper() {
        super(Storage.class, StorageDto.class);
        this.ignoredProperties("storageControllers", "drives", "links", "status");
    }

    @Override
    protected void performNotAutomatedMapping(Storage source, StorageDto target) {
        super.performNotAutomatedMapping(source, target);

        mapEnclosuresWithDrives(source, target);
        mapStatus(source, target);
    }

    private void mapEnclosuresWithDrives(Storage source, StorageDto target) {
        Collection<Context> enclosures = target.getLinks().getEnclosures() != null ? target.getLinks().getEnclosures() : new HashSet<>();
        Set<Context> drives = target.getDrives() != null ? target.getDrives() : new HashSet<>();
        if (source.getService() == null || !source.isComplementary()) {
            enclosures.addAll(asChassisContexts(source.getComputerSystem().getChassis()));
            for (Chassis enclosure : source.getComputerSystem().getChassis()) {
                drives.addAll(chassisSubResourcesFinder.getUniqueSubResourcesOfClass(enclosure, Drive.class).stream()
                    .map(Contexts::toContext)
                    .collect(toList()));
            }
        }
        target.getLinks().setEnclosures(enclosures);
        target.setDrives(drives);
    }

    private void mapStatus(Storage source, StorageDto target) {
        ofNullable(source.getStatus()).ifPresent(target::setStatus);
    }
}
