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

import com.intel.podm.business.dto.redfish.SwitchDto;
import com.intel.podm.redfish.json.templates.BaseJson;
import com.intel.podm.redfish.json.templates.SwitchJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

@SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
public class SwitchDtoJsonSerializer extends BaseDtoJsonSerializer<SwitchDto> {
    protected SwitchDtoJsonSerializer() {
        super(SwitchDto.class);
    }

    @Override
    protected BaseJson translate(SwitchDto dto) {
        SwitchJson switchJson = new SwitchJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());

        switchJson.oDataId = oDataId;
        switchJson.oDataContext = getContextFromId(oDataId);
        switchJson.id = dto.getId();
        switchJson.sku = dto.getSku();
        switchJson.name = dto.getName();
        switchJson.model = dto.getModel();
        switchJson.ports = oDataIdFromUri(create(oDataId + "/Ports"));
        switchJson.status = dto.getStatus();
        switchJson.assetTag = dto.getAssetTag();
        switchJson.domainId = dto.getDomainId();
        switchJson.isManaged = dto.getManaged();
        switchJson.partNumber = dto.getPartNumber();
        switchJson.switchType = dto.getSwitchType();
        switchJson.description = dto.getDescription();
        switchJson.manufacturer = dto.getManufacturer();
        switchJson.indicatorLed = dto.getIndicatorLed();
        switchJson.serialNumber = dto.getSerialNumber();
        switchJson.totalSwitchWidth = dto.getTotalSwitchWidth();
        switchJson.ports = oDataIdFromUri(create(oDataId + "/Ports"));
        switchJson.powerState = dto.getPowerState();
        switchJson.links.chassis.addAll(asOdataIdSet(dto.getLinks().getChassis()));
        switchJson.links.managedBy.addAll(asOdataIdSet(dto.getLinks().getManagedBy()));
        switchJson.actions.reset.target = oDataId + "/Actions/Switch.Reset";
        switchJson.actions.reset.allowableResetTypes.addAll(dto.getAllowableResetTypes());
        return switchJson;
    }
}
