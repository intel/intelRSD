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

import com.intel.podm.business.dto.redfish.LogicalDriveDto;
import com.intel.podm.redfish.json.templates.LogicalDriveJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.odataid.ODataIds;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;

public class LogicalDriveDtoJsonSerializer extends DtoJsonSerializer<LogicalDriveDto> {
    protected LogicalDriveDtoJsonSerializer() {
        super(LogicalDriveDto.class);
    }

    @Override
    protected LogicalDriveJson translate(LogicalDriveDto dto) {

        LogicalDriveJson logicalDriveJson = new LogicalDriveJson();
        ODataId oDataId = oDataId(context.getRequestPath());

        logicalDriveJson.oDataId = oDataId;
        logicalDriveJson.oDataContext = getContextFromId(oDataId);

        logicalDriveJson.id = dto.getId();
        logicalDriveJson.name = dto.getName();
        logicalDriveJson.type = dto.getType();
        logicalDriveJson.mode = dto.getMode();
        logicalDriveJson.image = dto.getImage();
        logicalDriveJson.status = dto.getStatus();
        logicalDriveJson.bootable = dto.getBootable();
        logicalDriveJson.snapshot = dto.getSnapshot();
        logicalDriveJson.capacityGib = dto.getCapacityGib();
        logicalDriveJson.description = dto.getDescription();
        logicalDriveJson.writeProtected = dto.getWriteProtected();

        mapLinks(dto, logicalDriveJson);

        return logicalDriveJson;
    }

    private void mapLinks(LogicalDriveDto dto, LogicalDriveJson logicalDriveJson) {
        logicalDriveJson.links.logicalDrives.addAll(ODataIds.oDataIdsCollection(dto.getLogicalDrives()));
        logicalDriveJson.links.physicalDrives.addAll(ODataIds.oDataIdsCollection(dto.getPhysicalDrives()));
        logicalDriveJson.links.usedBy.addAll(ODataIds.oDataIdsCollection(dto.getUsedBy()));
        logicalDriveJson.links.targets.addAll(ODataIds.oDataIdsCollection(dto.getTargets()));
        logicalDriveJson.links.masterDrive = ODataIds.oDataIdFromContext(dto.getMasterDrive());
    }
}

