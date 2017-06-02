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

import com.intel.podm.business.dto.redfish.StorageDto;
import com.intel.podm.business.dto.redfish.attributes.StorageControllerDto;
import com.intel.podm.redfish.json.templates.StorageJson;
import com.intel.podm.redfish.json.templates.attributes.IdentifierJson;
import com.intel.podm.redfish.json.templates.attributes.StorageControllerJson;
import com.intel.podm.business.services.redfish.odataid.ODataContextProvider;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.Collection;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
public class StorageDtoJsonSerializer extends BaseDtoJsonSerializer<StorageDto> {
    protected StorageDtoJsonSerializer() {
        super(StorageDto.class);
    }

    @Override
    protected StorageJson translate(StorageDto dto) {
        StorageJson storageJson = new StorageJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        storageJson.oDataId = oDataId;
        storageJson.oDataContext = ODataContextProvider.getContextFromId(oDataId);
        storageJson.id = dto.getId();
        storageJson.name = dto.getName();
        storageJson.status = dto.getStatus();
        storageJson.description = dto.getDescription();
        storageJson.drives = asOdataIdSet(dto.getDrives());
        storageJson.links.enclosures = asOdataIdSet(dto.getEnclosures());
        storageJson.storageControllers = getStorageControllersCollection(dto.getStorageControllers(), oDataId);
        return storageJson;
    }

    // OdataId as JsonPointer is being created manually here, as we does not have Context in StorageController to build OdataId from.
    private Collection<StorageControllerJson> getStorageControllersCollection(List<StorageControllerDto> storageControllers, ODataId oDataId) {
        return storageControllers.stream()
            .map(storageControllerDto -> {
                    int arrayIndex = storageControllers.indexOf(storageControllerDto);
                    String jsonPointer = oDataId.toString() + "#/StorageControllers/" + arrayIndex;
                    return toStorageController(storageControllerDto, jsonPointer);
                }
            )
            .collect(Collectors.toCollection(LinkedList::new));
    }

    private static StorageControllerJson toStorageController(StorageControllerDto storageController, String oDataId) {

        StorageControllerJson storageControllerJson = new StorageControllerJson();
        storageControllerJson.sku = storageController.getSku();
        storageControllerJson.model = storageController.getModel();
        storageControllerJson.status = storageController.getStatus();
        storageControllerJson.oDataId = oDataId;
        storageControllerJson.assetTag = storageController.getAssetTag();
        storageControllerJson.memberId = storageController.getMemberId();
        storageControllerJson.speedGbps = storageController.getSpeedGbps();
        storageControllerJson.partNumber = storageController.getPartNumber();
        storageControllerJson.serialNumber = storageController.getSerialNumber();
        storageControllerJson.manufacturer = storageController.getManufacturer();
        storageControllerJson.firmwareVersion = storageController.getFirmwareVersion();
        storageControllerJson.supportedDeviceProtocols = storageController.getSupportedDeviceProtocols();
        storageControllerJson.supportedControllerProtocols = storageController.getSupportedControllerProtocols();
        storageControllerJson.identifiers = storageController.getIdentifiers().stream()
            .map(identifier -> new IdentifierJson(identifier.getDurableName(), identifier.getDurableNameFormat()))
            .collect(toList());
        return storageControllerJson;

    }
}
