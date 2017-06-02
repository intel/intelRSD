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

import com.intel.podm.business.dto.redfish.PhysicalDriveDto;
import com.intel.podm.redfish.json.templates.PhysicalDriveJson;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

public class PhysicalDriveDtoJsonSerializer extends BaseDtoJsonSerializer<PhysicalDriveDto> {
    protected PhysicalDriveDtoJsonSerializer() {
        super(PhysicalDriveDto.class);
    }

    @Override
    protected PhysicalDriveJson translate(PhysicalDriveDto dto) {
        PhysicalDriveJson physicalDriveJson = new PhysicalDriveJson();

        physicalDriveJson.oDataId = oDataIdFromUri(context.getRequestPath());
        physicalDriveJson.oDataContext = getContextFromId(physicalDriveJson.oDataId);

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
        physicalDriveJson.links.usedBy.addAll(asOdataIdSet(dto.getLinks().getUsedBy()));

        return physicalDriveJson;
    }
}
