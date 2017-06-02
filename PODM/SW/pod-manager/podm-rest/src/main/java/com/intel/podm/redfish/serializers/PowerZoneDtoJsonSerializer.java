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

import com.intel.podm.business.dto.redfish.PowerZoneDto;
import com.intel.podm.business.dto.redfish.attributes.PowerZonePowerSupplyDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.redfish.json.templates.PowerZoneJson;
import com.intel.podm.redfish.json.templates.attributes.PowerZonePowerSupplyJson;
import com.intel.podm.redfish.json.templates.attributes.RackLocationAttributeJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.BaseDtoJsonSerializer;

import java.util.List;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromUri;
import static java.util.stream.Collectors.toList;

@SuppressWarnings({"checkstyle:ExecutableStatementCount"})
public class PowerZoneDtoJsonSerializer extends BaseDtoJsonSerializer<PowerZoneDto> {
    protected PowerZoneDtoJsonSerializer() {
        super(PowerZoneDto.class);
    }

    @Override
    protected PowerZoneJson translate(PowerZoneDto dto) {
        PowerZoneJson json = new PowerZoneJson();

        ODataId oDataId = oDataIdFromUri(context.getRequestPath());
        json.oDataId = oDataId;
        json.oDataContext = getContextFromId(oDataId);
        json.id = dto.getId();
        json.name = dto.getName();
        json.description = dto.getDescription();
        json.status = dto.getStatus();
        json.rackLocation = toRackLocation(dto.getRackLocation());
        json.maxPsusSupported = dto.getMaxPsusSupported();
        json.presence = dto.getPresence();
        json.numberOfPsusPresent = dto.getNumberOfPsusPresent();
        json.powerConsumedWatts = dto.getPowerConsumedWatts();
        json.powerOutputWatts = dto.getPowerOutputWatts();
        json.powerCapacityWatts = dto.getPowerCapacityWatts();

        addPowerSupplies(json, dto.getPowerSupplies());
        return json;
    }

    private void addPowerSupplies(PowerZoneJson json, List<PowerZonePowerSupplyDto> powerSupplies) {
        json.powerSupplies.addAll(powerSupplies.stream().map(this::createPowerSupply).collect(toList()));
    }

    private PowerZonePowerSupplyJson createPowerSupply(PowerZonePowerSupplyDto dto) {
        PowerZonePowerSupplyJson json = new PowerZonePowerSupplyJson();
        json.name = dto.getName();
        json.status = dto.getStatus();
        json.powerCapacityWatts = dto.getPowerCapacityWatts();
        json.lastPowerOutputWatts = dto.getLastPowerOutputWatt();
        json.manufacturer = dto.getManufacturer();
        json.firmwareRevision = dto.getFirmwareRevision();
        json.serialNumber = dto.getSerialNumber();
        json.modelNumber = dto.getModelNumber();
        json.partNumber = dto.getPartNumber();
        json.rackLocation = toRackLocation(dto.getRackLocation());

        return json;
    }

    private RackLocationAttributeJson toRackLocation(RackLocationDto rackLocation) {
        return rackLocation == null ? null : new RackLocationAttributeJson(
            rackLocation.getuLocation(),
            rackLocation.getuHeight(),
            rackLocation.getxLocation(),
            rackLocation.getRackUnitType());
    }
}
