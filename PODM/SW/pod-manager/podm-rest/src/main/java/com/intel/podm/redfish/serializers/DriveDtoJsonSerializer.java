/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.dto.redfish.DriveDto;
import com.intel.podm.common.types.redfish.DurableIdentifier;
import com.intel.podm.common.types.redfish.RedfishDrive;
import com.intel.podm.redfish.json.templates.DriveJson;
import com.intel.podm.redfish.json.templates.attributes.IdentifierJson;
import com.intel.podm.redfish.json.templates.attributes.RedfishLocationJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.List;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
public class DriveDtoJsonSerializer extends BaseDtoJsonSerializer<DriveDto> {
    protected DriveDtoJsonSerializer() {
        super(DriveDto.class);
    }

    @Override
    protected DriveJson translate(DriveDto dto) {
        DriveJson driveJson = new DriveJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        driveJson.oDataId = oDataId;
        driveJson.oDataContext = getContextFromId(oDataId);
        driveJson.id = dto.getId();

        driveJson.name = dto.getName();
        driveJson.description = dto.getDescription();
        driveJson.statusIndicator = dto.getStatusIndicator();
        driveJson.indicatorLed = dto.getIndicatorLed();
        driveJson.model = dto.getModel();
        driveJson.status = dto.getStatus();
        driveJson.revision = dto.getRevision();
        driveJson.capacityBytes = dto.getCapacityBytes();
        driveJson.failurePredicted = dto.getFailurePredicted();
        driveJson.protocol = dto.getProtocol();
        driveJson.mediaType = dto.getMediaType();
        driveJson.manufacturer = dto.getManufacturer();
        driveJson.sku = dto.getSku();
        driveJson.serialNumber = dto.getSerialNumber();
        driveJson.partNumber = dto.getPartNumber();
        driveJson.assetTag = dto.getAssetTag();
        driveJson.identifiers = mapToIdentifiers(dto.getIdentifiers());
        driveJson.location = mapToLocation(dto.getLocation());
        driveJson.hotspareType = dto.getHotspareType();
        driveJson.encryptionAbility = dto.getEncryptionAbility();
        driveJson.encryptionStatus = dto.getEncryptionStatus();
        driveJson.rotationSpeedRpm = dto.getRotationSpeedRpm();
        driveJson.blockSizeBytes = dto.getBlockSizeBytes();
        driveJson.capableSpeedGbs = dto.getCapableSpeedGbs();
        driveJson.negotiatedSpeedGbs = dto.getNegotiatedSpeedGbs();
        driveJson.predictedMediaLifeLeftPercent = dto.getPredictedMediaLifeLeftPercent();

        driveJson.oem.rackScaleOem.eraseOnDetach = dto.getEraseOnDetach();
        driveJson.oem.rackScaleOem.firmwareVersion = dto.getFirmwareVersion();
        driveJson.oem.rackScaleOem.driveErased = dto.getDriveErased();
        driveJson.oem.rackScaleOem.pcieFunction = asOdataId(dto.getPcieFunction());
        driveJson.oem.rackScaleOem.storage = asOdataId(dto.getStorage());

        driveJson.links.endpoints.addAll(asOdataIdSet(dto.getLinks().getEndpoints()));
        driveJson.links.volumes.addAll(asOdataIdSet(dto.getLinks().getVolumes()));
        driveJson.actions.secureEraseAction.target = oDataId + "/Actions/Drive.SecureErase";

        return driveJson;
    }


    private List<IdentifierJson> mapToIdentifiers(List<DurableIdentifier> identifiers) {
        return identifiers != null
            ?
            identifiers.stream()
                .map(identifier -> new IdentifierJson(identifier.getDurableName(), identifier.getDurableNameFormat()))
                .collect(toList())
            :
            emptyList();
    }

    private List<RedfishLocationJson> mapToLocation(List<RedfishDrive.Location> location) {
        return location != null
            ?
            location.stream()
                .map(item -> new RedfishLocationJson(item.getInfo(), item.getInfoFormat()))
                .collect(toList())
            :
            emptyList();
    }
}
