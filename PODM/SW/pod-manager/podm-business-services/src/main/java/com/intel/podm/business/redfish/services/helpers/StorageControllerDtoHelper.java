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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.business.dto.redfish.attributes.StorageControllerDto;
import com.intel.podm.business.entities.redfish.StorageController;

import javax.inject.Inject;

public final class StorageControllerDtoHelper {

    @Inject
    private IdentifierDtoHelper identifierDtoHelper;

    public StorageControllerDtoHelper() {
    }

    public StorageControllerDto createStorageControllerDto(StorageController storageController) {
        return StorageControllerDto.newBuilder()
            .firmwareVersion(storageController.getFirmwareVersion())
            .manufacturer(storageController.getManufacturer())
            .serialNumber(storageController.getSerialNumber())
            .partNumber(storageController.getPartNumber())
            .assetTag(storageController.getAssetTag())
            .model(storageController.getModel())
            .memberId(storageController.getMemberId())
            .sku(storageController.getSku())
            .status(storageController.getStatus())
            .speedGbps(storageController.getSpeedGbps())
            .identifiers(identifierDtoHelper.translateIdentifiersToDto(storageController.getIdentifiers()))
            .supportedDeviceProtocols(storageController.getSupportedDeviceProtocols())
            .supportedControllerProtocols(storageController.getSupportedControllerProtocols())
            .build();
    }
}
