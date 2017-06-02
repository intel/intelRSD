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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.DriveDto;
import com.intel.podm.business.dto.redfish.attributes.RedfishLocationDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.IdentifierDtoHelper;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;
import com.intel.podm.common.types.redfish.RedfishDrive;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.DRIVES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static com.intel.podm.business.redfish.Contexts.toContext;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:MethodLength"})
public class DriveServiceImpl implements ReaderService<DriveDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private IdentifierDtoHelper identifierDtoHelper;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context chassisContext) throws ContextResolvingException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);
        return new CollectionDto(DRIVES, getAsIdSet(chassis.getDrives()));
    }

    @Override
    public DriveDto getResource(Context context) throws ContextResolvingException {
        Drive drive = (Drive) traverser.traverse(context);
        return map(drive);
    }

    private DriveDto map(Drive drive) {
        return DriveDto.newBuilder()
            .id(drive.getId().toString())
            .name(drive.getName())
            .description(drive.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(drive.getService(), drive.getUnknownOems()))
            .sku(drive.getSku())
            .model(drive.getModel())
            .status(drive.getStatus())
            .location(getLocation(drive))
            .assetTag(drive.getAssetTag())
            .protocol(drive.getProtocol())
            .revision(drive.getRevision())
            .mediaType(drive.getMediaType())
            .partNumber(drive.getPartNumber())
            .identifiers(identifierDtoHelper.translateIdentifiersToDto(drive.getIdentifiers()))
            .hotspareType(drive.getHotspareType())
            .manufacturer(drive.getManufacturer())
            .indicatorLed(drive.getIndicatorLed())
            .serialNumber(drive.getSerialNumber())
            .eraseOnDetach(drive.getEraseOnDetach())
            .capacityBytes(drive.getCapacityBytes())
            .endpoints(toContext(drive.getConnectedEntity() != null ? drive.getConnectedEntity().getEndpoint() : null))
            .pcieFunction(toContext(drive.getPcieDeviceFunction()))
            .storage(toContext(drive.getStorage()))
            .blockSizeBytes(drive.getBlockSizeBytes())
            .capableSpeedGbs(drive.getCapableSpeedGbs())
            .statusIndicator(drive.getStatusIndicator())
            .encryptionStatus(drive.getEncryptionStatus())
            .rotationSpeedRpm(drive.getRotationSpeedRpm())
            .failurePredicted(drive.getFailurePredicted())
            .encryptionAbility(drive.getEncryptionAbility())
            .negotiatedSpeedGbs(drive.getNegotiatedSpeedGbs())
            .predictedMediaLifeLeftPercent(drive.getPredictedMediaLifeLeftPercent())
            .firmwareVersion(drive.getFirmwareVersion())
            .driveErased(drive.getDriveErased())
            .build();
    }

    private List<RedfishDrive.Location> getLocation(Drive drive) {
        return drive.getLocations().stream()
            .map(location -> RedfishLocationDto.newBuilder()
                .info(location.getInfo())
                .infoFormat(location.getInfoFormat())
                .build())
            .collect(toList());
    }
}
