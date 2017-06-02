/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.List;

import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static com.intel.podm.rest.representation.json.serializers.OemSerializeHelper.oemDtoToJsonNode;
import static java.util.stream.Collectors.toList;

public class SimpleStorageDtoJsonSerializer extends BaseDtoJsonSerializer<SimpleStorageDto> {
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
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        simpleStorageJson.oDataId = oDataId;
        simpleStorageJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        simpleStorageJson.description = dto.getDescription();
        simpleStorageJson.uefiDevicePath = dto.getUefiDevicePath();

        return simpleStorageJson;
    }

    private List<SimpleStorageDeviceJson> toSimpleStorageDeviceJsonCollection(List<SimpleStorageDeviceDto> devices) {
        return devices.stream()
            .map(simpleStorageDevice -> {
                    SimpleStorageDeviceJson simpleStorageDeviceJson = new SimpleStorageDeviceJson();
                    simpleStorageDeviceJson.name = simpleStorageDevice.getName();
                    simpleStorageDeviceJson.manufacturer = simpleStorageDevice.getManufacturer();
                    simpleStorageDeviceJson.model = simpleStorageDevice.getModel();
                    simpleStorageDeviceJson.status = simpleStorageDevice.getStatus();
                    simpleStorageDeviceJson.capacityBytes = simpleStorageDevice.getCapacityBytes();
                    simpleStorageDeviceJson.oem = oemDtoToJsonNode(simpleStorageDevice.getOem());
                    return simpleStorageDeviceJson;
                }
            ).collect(toList());
    }
}
