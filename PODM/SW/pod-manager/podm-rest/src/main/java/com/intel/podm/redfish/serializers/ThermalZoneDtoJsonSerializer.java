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

import com.intel.podm.business.dto.redfish.ThermalZoneDto;
import com.intel.podm.business.dto.redfish.attributes.FanDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.dto.redfish.attributes.TemperatureDto;
import com.intel.podm.redfish.json.templates.ThermalZoneJson;
import com.intel.podm.redfish.json.templates.attributes.RackLocationAttributeJson;
import com.intel.podm.rest.odataid.ODataId;
import com.intel.podm.rest.representation.json.serializers.DtoJsonSerializer;

import static com.intel.podm.rest.odataid.ODataContextProvider.getContextFromId;
import static java.util.Objects.isNull;
import static java.util.stream.Collectors.toList;

public class ThermalZoneDtoJsonSerializer extends DtoJsonSerializer<ThermalZoneDto> {

    public ThermalZoneDtoJsonSerializer() {
        super(ThermalZoneDto.class);
    }

    @Override
    protected ThermalZoneJson translate(ThermalZoneDto dto) {
        ThermalZoneJson thermalZoneJson = new ThermalZoneJson();

        ODataId oDataId = ODataId.oDataId(context.getRequestPath());
        thermalZoneJson.oDataId = oDataId;
        thermalZoneJson.oDataContext = getContextFromId(oDataId);

        thermalZoneJson.id = dto.getId();
        thermalZoneJson.name = dto.getName();
        thermalZoneJson.fans.addAll(dto.getFans().stream().map(this::toFanJson).collect(toList()));
        thermalZoneJson.status = dto.getStatus();
        thermalZoneJson.presence = dto.getPresence();
        thermalZoneJson.description = dto.getDescription();
        thermalZoneJson.rackLocation = toRackLocationAttributeJson(dto.getRackLocation());
        thermalZoneJson.temperatures.addAll(dto.getTemperatures().stream().map(this::toTemperatureJson).collect(toList()));
        thermalZoneJson.desiredSpeedPwm = dto.getDesiredSpeedPwm();
        thermalZoneJson.desiredSpeedRpm = dto.getDesiredSpeedRpm();
        thermalZoneJson.maxFansSupported = dto.getMaxFansSupported();
        thermalZoneJson.volumetricAirflow = dto.getVolumetricAirflow();
        thermalZoneJson.numberOfFansPresent = dto.getNumberOfFansPresent();

        return thermalZoneJson;
    }

    private ThermalZoneJson.Fan toFanJson(FanDto fanDto) {
        ThermalZoneJson.Fan fan = new ThermalZoneJson.Fan();

        fan.name = fanDto.getName();
        fan.status = fanDto.getStatus();
        fan.readingRpm = fanDto.getReadingRpm();
        fan.rackLocation = toRackLocationAttributeJson(fanDto.getRackLocation());

        return fan;
    }

    private ThermalZoneJson.Temperature toTemperatureJson(TemperatureDto temperatureDto) {
        ThermalZoneJson.Temperature temperature = new ThermalZoneJson.Temperature();

        temperature.name = temperatureDto.getName();
        temperature.status = temperatureDto.getStatus();
        temperature.readingCelsius = temperatureDto.getReadingCelsius();
        temperature.physicalContext = temperatureDto.getPhysicalContext();

        return temperature;
    }

    private RackLocationAttributeJson toRackLocationAttributeJson(RackLocationDto rackLocationDto) {
        return isNull(rackLocationDto) ? null : new RackLocationAttributeJson(
                rackLocationDto.getuLocation(),
                rackLocationDto.getuHeight(),
                rackLocationDto.getxLocation(),
                rackLocationDto.getRackUnitType()
        );
    }
}
