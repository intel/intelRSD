/*
 * Copyright (c) 2016 Intel Corporation
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
import com.intel.podm.business.dto.redfish.attributes.PowerSupplyDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.redfish.json.templates.PowerZoneJson;
import com.intel.podm.redfish.json.templates.attributes.PowerSupplyJson;
import com.intel.podm.redfish.json.templates.attributes.RackLocationAttributeJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import java.util.List;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static com.intel.podm.rest.odataid.ODataId.oDataId;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

public class PowerZoneDtoJsonSerializer extends DtoJsonSerializer<PowerZoneDto> {

    protected PowerZoneDtoJsonSerializer() {
        super(PowerZoneDto.class);
    }

    @Override
    protected PowerZoneJson translate(PowerZoneDto dto) {
        PowerZoneJson json = new PowerZoneJson();

        ODataId oDataId = oDataId(context.getRequestPath());
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

    private void addPowerSupplies(PowerZoneJson json, List<PowerSupplyDto> powerSupplies) {
        json.powerSupplies.addAll(powerSupplies.stream().map(this::createPowerSupply).collect(toList()));
    }

    private PowerSupplyJson createPowerSupply(PowerSupplyDto dto) {
        PowerSupplyJson json = new PowerSupplyJson();
        json.name = dto.getName();
        json.status = dto.getStatus();
        json.powerCapacityWatts = dto.getPowerCapacityWatts();
        json.lastPowerOutputWatts = dto.getLastPowerOutputWatt();
        json.manufacturer = dto.getManufacturer();
        json.firmwareVersion = dto.getFirmwareVersion();
        json.serialNumber = dto.getSerialNumber();
        json.model = dto.getModel();
        json.partNumber = dto.getPartNumber();
        json.rackLocation = toRackLocation(dto.getRackLocation());

        return json;
    }

    private RackLocationAttributeJson toRackLocation(RackLocationDto rackLocation) {
        return isNull(rackLocation) ? null : new RackLocationAttributeJson(
                rackLocation.getuLocation(),
                rackLocation.getuHeight(),
                rackLocation.getxLocation(),
                rackLocation.getRackUnitType());
    }
}
