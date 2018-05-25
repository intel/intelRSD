/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.dto.PcieDeviceFunctionDto;
import com.intel.podm.business.dto.StorageControllerDto;
import com.intel.podm.business.entities.redfish.PcieDeviceFunction;
import com.intel.podm.business.entities.redfish.StorageController;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Set;

import static com.intel.podm.business.redfish.ContextCollections.asDriveContexts;
import static com.intel.podm.business.redfish.ContextCollections.asEthernetInterfaceContexts;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toSet;

@Dependent
class PcieDeviceFunctionDtoMapper extends DtoMapper<PcieDeviceFunction, PcieDeviceFunctionDto> {
    @Inject
    private EntityToDtoMapper entityToDtoMapper;

    PcieDeviceFunctionDtoMapper() {
        super(PcieDeviceFunction.class, PcieDeviceFunctionDto.class);
        this.ignoredProperties("links", "oem");
    }

    @Override
    protected void performNotAutomatedMapping(PcieDeviceFunction source, PcieDeviceFunctionDto target) {
        super.performNotAutomatedMapping(source, target);

        target.getLinks().setPcieDevice(toContext(source.getPcieDevice()));
        target.getLinks().getDrives().addAll(asDriveContexts(source.getDrives()));
        target.getLinks().getEthernetInterfaces().addAll(asEthernetInterfaceContexts(source.getEthernetInterfaces()));
        target.getLinks().getStorageControllers().addAll(mapStorageControllers(source.getStorageControllers()));
    }

    private Set<StorageControllerDto> mapStorageControllers(Set<StorageController> controllers) {
        return controllers.stream()
            .map(storageController -> (StorageControllerDto) entityToDtoMapper.map(storageController))
            .collect(toSet());
    }
}
