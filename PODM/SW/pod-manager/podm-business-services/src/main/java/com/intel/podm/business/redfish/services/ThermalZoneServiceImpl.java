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
import com.intel.podm.business.dto.redfish.ThermalZoneDto;
import com.intel.podm.business.dto.redfish.attributes.FanDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.dto.redfish.attributes.TemperatureDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.Fan;
import com.intel.podm.business.entities.redfish.Temperature;
import com.intel.podm.business.entities.redfish.ThermalZone;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.THERMAL_ZONES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ThermalZoneServiceImpl implements ReaderService<ThermalZoneDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context context) throws ContextResolvingException {
        Chassis chassis = (Chassis) traverser.traverse(context);
        return new CollectionDto(THERMAL_ZONES, getAsIdSet(chassis.getThermalZones()));
    }

    @Override
    public ThermalZoneDto getResource(Context context) throws ContextResolvingException {
        ThermalZone thermalZone = (ThermalZone) traverser.traverse(context);
        return toThermalZoneDto(thermalZone);
    }

    private ThermalZoneDto toThermalZoneDto(ThermalZone thermalZone) {
        return ThermalZoneDto.newBuilder()
            .id(thermalZone.getId().toString())
            .name(thermalZone.getName())
            .description(thermalZone.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(thermalZone.getService(), thermalZone.getUnknownOems()))
            .rackLocation(toRackLocation(thermalZone.getRackLocation()))
            .presence(thermalZone.getPresence())
            .desiredSpeedPwm(thermalZone.getDesiredSpeedPwm())
            .desiredSpeedRpm(thermalZone.getDesiredSpeedRpm())
            .maxFansSupported(thermalZone.getMaxFansSupported())
            .numberOfFansPresent(thermalZone.getNumberOfFansPresent())
            .volumetricAirflow(thermalZone.getVolumetricAirflow())
            .temperatures(thermalZone.getTemperatures().stream().map(this::toTemperatureDto).collect(toList()))
            .status(thermalZone.getStatus())
            .fans(thermalZone.getFans().stream().map(this::toFanDto).collect(toList()))
            .build();
    }

    private TemperatureDto toTemperatureDto(Temperature temperature) {
        return TemperatureDto.newBuilder()
            .status(temperature.getStatus())
            .name(temperature.getName())
            .readingCelsius(temperature.getReadingCelsius())
            .physicalContext(temperature.getPhysicalContext())
            .build();
    }

    private FanDto toFanDto(Fan fan) {
        return FanDto.newBuilder()
            .name(fan.getName())
            .status(fan.getStatus())
            .rackLocationDto(toRackLocation(fan.getRackLocation()))
            .readingRpm(fan.getReadingRpm())
            .build();
    }

    private RackLocationDto toRackLocation(RackLocation rackLocation) {
        return rackLocation == null ? null : RackLocationDto.newBuilder()
            .rackUnitType(rackLocation.getRackUnit())
            .uHeight(rackLocation.getUHeight())
            .uLocation(rackLocation.getULocation())
            .xLocation(rackLocation.getXLocation())
            .build();
    }
}
