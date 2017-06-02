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

import com.intel.podm.business.dto.redfish.LogicalDriveDto;
import com.intel.podm.redfish.json.templates.LogicalDriveJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public class LogicalDriveDtoJsonSerializer extends BaseDtoJsonSerializer<LogicalDriveDto> {
    protected LogicalDriveDtoJsonSerializer() {
        super(LogicalDriveDto.class);
    }

    @Override
    protected LogicalDriveJson translate(LogicalDriveDto dto) {

        LogicalDriveJson logicalDriveJson = new LogicalDriveJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

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

        mapLinks(dto.getLinks(), logicalDriveJson);

        return logicalDriveJson;
    }

    private void mapLinks(LogicalDriveDto.Links links, LogicalDriveJson logicalDriveJson) {
        logicalDriveJson.links.logicalDrives.addAll(asOdataIdSet(links.getLogicalDrives()));
        logicalDriveJson.links.physicalDrives.addAll(asOdataIdSet(links.getPhysicalDrives()));
        logicalDriveJson.links.usedBy.addAll(asOdataIdSet(links.getUsedBy()));
        logicalDriveJson.links.targets.addAll(asOdataIdSet(links.getTargets()));
        logicalDriveJson.links.masterDrive = asOdataId(links.getMasterDrive());
    }
}

