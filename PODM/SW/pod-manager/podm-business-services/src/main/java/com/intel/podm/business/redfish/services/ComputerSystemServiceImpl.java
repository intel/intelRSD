/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.ComputerSystemDto;
import com.intel.podm.business.dto.redfish.attributes.SystemDeviceDto;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.properties.ComputerSystemDevice;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.BootDtoHelper;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ComputerSystemService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.SYSTEMS;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class ComputerSystemServiceImpl implements ComputerSystemService {
    @Inject
    private GenericDao genericDao;

    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private BootDtoHelper bootDtoHelper;

    @Override
    public CollectionDto getComputerSystemCollection() {
        Collection<ComputerSystem> computerSystemCollection = genericDao.findAll(ComputerSystem.class);
        return new CollectionDto(SYSTEMS, Contexts.getAsIdList(computerSystemCollection));
    }

    @Override
    public ComputerSystemDto getComputerSystem(Context system) throws EntityNotFoundException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(system);

        return ComputerSystemDto
                .newBuilder()
                .id(computerSystem.getId())
                .name(computerSystem.getName())
                .description(computerSystem.getDescription())
                .assetTag(computerSystem.getAssetTag())
                .biosVersion(computerSystem.getBiosVersion())
                .sku(computerSystem.getSku())
                .hostName(computerSystem.getHostName())
                .indicatorLed(computerSystem.getIndicatorLed())
                .model(computerSystem.getModel())
                .memorySockets(computerSystem.getMemorySockets())
                .memoryStatus(computerSystem.getMemoryStatus())
                .totalSystemMemoryGiB(computerSystem.getTotalSystemMemoryGiB())
                .uuid(computerSystem.getUuid())
                .manufacturer(computerSystem.getManufacturer())
                .processorsCount(computerSystem.getProcessorsCount())
                .processorsModel(computerSystem.getProcessorsModel())
                .processorSockets(computerSystem.getProcessorsSockets())
                .processorsStatus(computerSystem.getProcessorsStatus())
                .chassis(Contexts.asChassisContexts(computerSystem.getChassis()))
                .boot(bootDtoHelper.buildBootDto(computerSystem.getBoot()))
                .discoveryState(computerSystem.getDiscoveryState())
                .status(computerSystem.getStatus())
                .systemType(computerSystem.getSystemType())
                .powerState(computerSystem.getPowerState())
                .serialNumber(computerSystem.getSerialNumber())
                .partNumber(computerSystem.getPartNumber())
                .chassis(Contexts.asChassisContexts(computerSystem.getChassis()))
                .pciDevices(buildDevices(computerSystem.getPciDevices()))
                .allowableResetTypes(computerSystem.getAllowableResetTypes())
                .managedBy(Contexts.asManagerContexts(computerSystem.getManagers()))
                .build();
    }

    private List<SystemDeviceDto> buildDevices(Collection<ComputerSystemDevice> devices) {
        return devices.stream()
                .map(device -> new SystemDeviceDto(device.getVendorId(), device.getDeviceId()))
                .collect(toList());
    }
}
