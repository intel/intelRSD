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

import com.intel.podm.business.dto.redfish.PcieDeviceDto;
import com.intel.podm.redfish.json.templates.PcieDeviceJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;

@SuppressWarnings({"checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public class PcieDeviceJsonSerializer extends BaseDtoJsonSerializer<PcieDeviceDto> {
    public PcieDeviceJsonSerializer() {
        super(PcieDeviceDto.class);
    }

    @Override
    protected PcieDeviceJson translate(PcieDeviceDto dto) {
        PcieDeviceJson pcieDeviceJson = new PcieDeviceJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        pcieDeviceJson.oDataId = oDataId;
        pcieDeviceJson.oDataContext = getContextFromId(oDataId);

        pcieDeviceJson.id = dto.getId();
        pcieDeviceJson.name = dto.getName();
        pcieDeviceJson.description = dto.getDescription();
        pcieDeviceJson.assetTag = dto.getAssetTag();
        pcieDeviceJson.manufacturer = dto.getManufacturer();
        pcieDeviceJson.model = dto.getModel();
        pcieDeviceJson.sku = dto.getSku();
        pcieDeviceJson.serialNumber = dto.getSerialNumber();
        pcieDeviceJson.partNumber = dto.getPartNumber();
        pcieDeviceJson.deviceType = dto.getDeviceType();
        pcieDeviceJson.firmwareVersion = dto.getFirmwareVersion();
        pcieDeviceJson.status = dto.getStatus();

        pcieDeviceJson.links.chassis.addAll(asOdataIdSet(dto.getLinks().getChassis()));
        pcieDeviceJson.links.pcieFunctions.addAll(asOdataIdSet(dto.getLinks().getPcieFunctions()));

        return pcieDeviceJson;
    }
}
