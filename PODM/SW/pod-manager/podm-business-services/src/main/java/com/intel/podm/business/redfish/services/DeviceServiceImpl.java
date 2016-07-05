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
import com.intel.podm.business.dto.redfish.DeviceDto;
import com.intel.podm.business.entities.redfish.Adapter;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.DeviceService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DEVICES;
import static com.intel.podm.business.redfish.Contexts.getAsIdList;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class DeviceServiceImpl implements DeviceService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getDevices(Context context) throws EntityNotFoundException {
        Adapter adapter = (Adapter) traverser.traverse(context);
        return new CollectionDto(DEVICES, getAsIdList(adapter.getDevices()));
    }

    @Override
    public DeviceDto getDevice(Context context) throws EntityNotFoundException {
        return toDeviceDto((Device) traverser.traverse(context));
    }

    private static DeviceDto toDeviceDto(Device device) {
        return DeviceDto.newBuilder()
                .id(device.getId().toString())
                .name(device.getName())
                .description(device.getDescription())
                .controllerInterface(device.getInterface())
                .capacityGib(device.getCapacityGib())
                .type(device.getType())
                .rpm(device.getRpm())
                .manufacturer(device.getManufacturer())
                .model(device.getModel())
                .serialNumber(device.getSerialNumber())
                .firmwareVersion(device.getFirmwareVersion())
                .busInfo(device.getBusInfo())
                .status(device.getStatus())
                .containedBy(toContext(device.getAdapter()))
                .build();
    }
}
