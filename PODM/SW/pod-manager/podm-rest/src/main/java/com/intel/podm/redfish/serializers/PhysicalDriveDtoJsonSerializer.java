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

import com.intel.podm.business.dto.redfish.PhysicalDriveDto;
import com.intel.podm.redfish.json.templates.PhysicalDriveJson;
import com.intel.podm.rest.odataid.ODataContextProvider;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static com.intel.podm.rest.odataid.ODataIds.oDataIdsCollection;

public class PhysicalDriveDtoJsonSerializer extends DtoJsonSerializer<PhysicalDriveDto> {

    protected PhysicalDriveDtoJsonSerializer() {
        super(PhysicalDriveDto.class);
    }

    @Override
    protected PhysicalDriveJson translate(PhysicalDriveDto dto) {
        PhysicalDriveJson physicalDriveJson = new PhysicalDriveJson();

        physicalDriveJson.oDataId = oDataId(context.getRequestPath());
        physicalDriveJson.oDataContext = ODataContextProvider.getContextFromId(physicalDriveJson.oDataId);

        physicalDriveJson.id = dto.getId();
        physicalDriveJson.rpm = dto.getRpm();
        physicalDriveJson.name = dto.getName();
        physicalDriveJson.description = dto.getDescription();
        physicalDriveJson.type = dto.getType();
        physicalDriveJson.model = dto.getModel();
        physicalDriveJson.status = dto.getStatus();
        physicalDriveJson.capacityGib = dto.getCapacityGib();
        physicalDriveJson.manufacturer = dto.getManufacturer();
        physicalDriveJson.serialNumber = dto.getSerialNumber();
        physicalDriveJson.controllerInterface = dto.getControllerInterface();
        physicalDriveJson.links.usedBy.addAll(oDataIdsCollection(dto.getUsedBy()));

        return physicalDriveJson;
    }
}
