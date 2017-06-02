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

import com.intel.podm.business.dto.redfish.EthernetSwitchDto;
import com.intel.podm.redfish.json.templates.EthernetSwitchJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataIdSet;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.net.URI.create;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public class EthernetSwitchJsonSerializer extends BaseDtoJsonSerializer<EthernetSwitchDto> {
    protected EthernetSwitchJsonSerializer() {
        super(EthernetSwitchDto.class);
    }

    @Override
    protected EthernetSwitchJson translate(EthernetSwitchDto dto) {
        EthernetSwitchJson switchJson = new EthernetSwitchJson();
        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        switchJson.oDataId = oDataId;
        switchJson.oDataContext = getContextFromId(oDataId);

        switchJson.id = dto.getId();
        switchJson.switchId = dto.getSwitchId();
        switchJson.name = dto.getName();
        switchJson.description = dto.getDescription();
        switchJson.manufacturer = dto.getManufacturer();
        switchJson.model = dto.getModel();
        switchJson.manufacturingDate = dto.getManufacturingDate();
        switchJson.serialNumber = dto.getSerialNumber();
        switchJson.partNumber = dto.getPartNumber();
        switchJson.firmwareName = dto.getFirmwareName();
        switchJson.firmwareVersion = dto.getFirmwareVersion();
        switchJson.role = dto.getRole();
        switchJson.status = dto.getStatus();
        switchJson.ports = oDataIdFromUri(create(oDataId + "/Ports"));
        switchJson.links.chassis = asOdataId(dto.getLinks().getChassis());
        switchJson.links.managedBy = asOdataIdSet(dto.getLinks().getManagedBy());

        return switchJson;
    }
}
