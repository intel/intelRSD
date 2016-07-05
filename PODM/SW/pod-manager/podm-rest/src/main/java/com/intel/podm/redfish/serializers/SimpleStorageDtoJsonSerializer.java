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

import com.intel.podm.business.dto.redfish.SimpleStorageDto;
import com.intel.podm.business.dto.redfish.SimpleStorageDto.SimpleStorageDeviceDto;
import com.intel.podm.redfish.json.templates.SimpleStorageJson;
import com.intel.podm.redfish.json.templates.attributes.SimpleStorageDeviceJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.List;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static java.util.stream.Collectors.toList;

public class SimpleStorageDtoJsonSerializer extends DtoJsonSerializer<SimpleStorageDto> {
    protected SimpleStorageDtoJsonSerializer() {
        super(SimpleStorageDto.class);
    }

    @Override
    protected SimpleStorageJson translate(SimpleStorageDto dto) {
        SimpleStorageJson simpleStorageJson = new SimpleStorageJson();

        simpleStorageJson.id = dto.getId();
        simpleStorageJson.name = dto.getName();
        simpleStorageJson.description = dto.getDescription();
        simpleStorageJson.status = dto.getStatus();
        simpleStorageJson.devices = toSimpleStorageDeviceJsonCollection(dto.getDevices());
        ODataId oDataId = ODataId.oDataId(context.getRequestPath());
        simpleStorageJson.oDataId = oDataId;
        simpleStorageJson.oDataContext = getContextFromId(oDataId);
        simpleStorageJson.description = dto.getDescription();
        simpleStorageJson.uefiDevicePath = dto.getUefiDevicePath();

        return simpleStorageJson;
    }

    public List<SimpleStorageDeviceJson> toSimpleStorageDeviceJsonCollection(List<SimpleStorageDeviceDto> devices) {
        return devices.stream()
                .map(simpleStorageDevice -> new SimpleStorageDeviceJson(
                                simpleStorageDevice.getName(),
                                simpleStorageDevice.getManufacturer(),
                                simpleStorageDevice.getModel(),
                                simpleStorageDevice.getStatus(),
                                simpleStorageDevice.getCapacityBytes())
                ).collect(toList());
    }
}
