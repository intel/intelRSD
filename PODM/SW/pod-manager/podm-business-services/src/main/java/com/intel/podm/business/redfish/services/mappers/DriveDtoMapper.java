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

import com.intel.podm.business.dto.DriveDto;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.Set;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.Optional.ofNullable;

@Dependent
class DriveDtoMapper extends DtoMapper<Drive, DriveDto> {
    DriveDtoMapper() {
        super(Drive.class, DriveDto.class);
        this.ignoredProperties("links", "actions", "oem", "status");
    }

    @Override
    protected void performNotAutomatedMapping(Drive source, DriveDto target) {
        super.performNotAutomatedMapping(source, target);

        DriveDto.Oem.RackScaleOem rackScaleOem = target.getOem().getRackScaleOem();
        rackScaleOem.setDriveErased(source.getDriveErased());
        rackScaleOem.setEraseOnDetach(source.getEraseOnDetach());
        rackScaleOem.setFirmwareVersion(source.getFirmwareVersion());
        rackScaleOem.setPcieFunction(toContext(source.getPcieDeviceFunction()));
        rackScaleOem.setStorage(toContext(source.getStorage()));
        if (!source.isComplementary()) {
            DriveDto.Links links = target.getLinks();
            links.setVolumes(new HashSet<>());

            Set<Context> endpoints = links.getEndpoints() != null ? links.getEndpoints() : new LinkedHashSet<>();
            if (source.getConnectedEntity() != null) {
                endpoints.add(toContext(source.getConnectedEntity().getEndpoint()));
            }
            links.setEndpoints(endpoints);
            mapStatus(source, target);
        }
    }

    private void mapStatus(Drive source, DriveDto target) {
        ofNullable(source.getStatus()).ifPresent(target::setStatus);
    }
}
