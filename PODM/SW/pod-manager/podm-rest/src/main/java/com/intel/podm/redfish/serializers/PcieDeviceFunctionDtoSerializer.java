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

import com.intel.podm.business.dto.redfish.PcieDeviceFunctionDto;
import com.intel.podm.business.dto.redfish.attributes.StorageControllerDto;
import com.intel.podm.redfish.json.templates.PcieDeviceFunctionJson;
import com.intel.podm.redfish.json.templates.attributes.IdentifierJson;
import com.intel.podm.redfish.json.templates.attributes.StorageControllerJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.function.Function;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.stream.Collectors.toSet;

@SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
public class PcieDeviceFunctionDtoSerializer extends BaseDtoJsonSerializer<PcieDeviceFunctionDto> {
    protected PcieDeviceFunctionDtoSerializer() {
        super(PcieDeviceFunctionDto.class);
    }

    @Override
    protected PcieDeviceFunctionJson translate(PcieDeviceFunctionDto pcieDeviceFunctionDto) {
        PcieDeviceFunctionJson pcieDeviceFunctionJson = new PcieDeviceFunctionJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        pcieDeviceFunctionJson.oDataId = oDataId;
        pcieDeviceFunctionJson.oDataContext = getContextFromId(oDataId);

        pcieDeviceFunctionJson.id = pcieDeviceFunctionDto.getId();
        pcieDeviceFunctionJson.name = pcieDeviceFunctionDto.getName();
        pcieDeviceFunctionJson.description = pcieDeviceFunctionDto.getDescription();
        pcieDeviceFunctionJson.functionId = pcieDeviceFunctionDto.getFunctionId();
        pcieDeviceFunctionJson.deviceClass = pcieDeviceFunctionDto.getDeviceClass();
        pcieDeviceFunctionJson.functionType = pcieDeviceFunctionDto.getFunctionType();
        pcieDeviceFunctionJson.deviceId = pcieDeviceFunctionDto.getDeviceId();
        pcieDeviceFunctionJson.vendorId = pcieDeviceFunctionDto.getVendorId();
        pcieDeviceFunctionJson.classCode = pcieDeviceFunctionDto.getClassCode();
        pcieDeviceFunctionJson.revisionId = pcieDeviceFunctionDto.getRevisionId();
        pcieDeviceFunctionJson.subsystemId = pcieDeviceFunctionDto.getSubsystemId();
        pcieDeviceFunctionJson.subsystemVendorId = pcieDeviceFunctionDto.getSubsystemVendorId();
        pcieDeviceFunctionJson.status = pcieDeviceFunctionDto.getStatus();

        pcieDeviceFunctionJson.links.drives.addAll(asOdataIdSet(pcieDeviceFunctionDto.getLinks().getDrives()));
        pcieDeviceFunctionJson.links.storageControllers = pcieDeviceFunctionDto.getLinks()
            .getStorageControllers().stream()
            .map(toStorageController(oDataId))
            .collect(toSet());
        pcieDeviceFunctionJson.links.ethernetInterfaces.addAll(asOdataIdSet(pcieDeviceFunctionDto.getLinks().getEthernetInterfaces()));
        pcieDeviceFunctionJson.links.pcieDevice = asOdataId(pcieDeviceFunctionDto.getLinks().getPcieDevice());

        pcieDeviceFunctionJson.status = pcieDeviceFunctionDto.getStatus();

        return pcieDeviceFunctionJson;
    }

    private static Function<StorageControllerDto, StorageControllerJson> toStorageController(ODataId oDataId) {
        return storageController -> {
            StorageControllerJson storageControllerJson = new StorageControllerJson();
            storageControllerJson.sku = storageController.getSku();
            storageControllerJson.model = storageController.getModel();
            storageControllerJson.status = storageController.getStatus();
            storageControllerJson.oDataId = oDataId.toString() + "#/StorageControllers/" + storageController.getMemberId();
            storageControllerJson.assetTag = storageController.getAssetTag();
            storageControllerJson.speedGbps = storageController.getSpeedGbps();
            storageControllerJson.memberId = storageController.getMemberId();
            storageControllerJson.partNumber = storageController.getPartNumber();
            storageControllerJson.serialNumber = storageController.getSerialNumber();
            storageControllerJson.manufacturer = storageController.getManufacturer();
            storageControllerJson.firmwareVersion = storageController.getFirmwareVersion();
            storageControllerJson.supportedDeviceProtocols = storageController.getSupportedDeviceProtocols();
            storageControllerJson.supportedControllerProtocols = storageController.getSupportedControllerProtocols();
            storageControllerJson.identifiers = storageController.getIdentifiers().stream()
                .map(identifier -> new IdentifierJson(identifier.getDurableName(), identifier.getDurableNameFormat()))
                .collect(toSet());
            return storageControllerJson;
        };
    }
}
