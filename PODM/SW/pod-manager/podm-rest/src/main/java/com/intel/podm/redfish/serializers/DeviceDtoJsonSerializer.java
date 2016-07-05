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

package com.intel.podm.redfish.serializers;

import com.intel.podm.business.dto.redfish.DeviceDto;
import com.intel.podm.redfish.json.templates.DeviceJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdFromContext;

public class DeviceDtoJsonSerializer extends DtoJsonSerializer<DeviceDto> {
    public DeviceDtoJsonSerializer() {
        super(DeviceDto.class);
    }

    @Override
    protected DeviceJson translate(DeviceDto dto) {
        DeviceJson driveJson = new DeviceJson();

        ODataId oDataId = oDataId(context.getRequestPath());
        driveJson.oDataId = oDataId;
        driveJson.oDataContext = getContextFromId(oDataId);

        driveJson.id = dto.getId();
        driveJson.rpm = dto.getRpm();
        driveJson.name = dto.getName();
        driveJson.description = dto.getDescription();
        driveJson.type = dto.getType();
        driveJson.model = dto.getModel();
        driveJson.status = dto.getStatus();
        driveJson.busInfo = dto.getBusInfo();
        driveJson.capacityGib = dto.getCapacityGib();
        driveJson.serialNumber = dto.getSerialNumber();
        driveJson.manufacturer = dto.getManufacturer();
        driveJson.firmwareVersion = dto.getFirmwareVersion();
        driveJson.links.containedBy = oDataIdFromContext(dto.getContainedBy());
        driveJson.controllerInterface = dto.getControllerInterface();

        return driveJson;
    }
}
