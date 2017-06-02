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
import com.intel.podm.business.dto.redfish.PowerZoneDto;
import com.intel.podm.business.dto.redfish.attributes.PowerZonePowerSupplyDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.PowerZonePowerSupply;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ReaderService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.POWER_ZONES;
import static com.intel.podm.business.redfish.ContextCollections.getAsIdSet;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PowerZoneServiceImpl implements ReaderService<PowerZoneDto> {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    private UnknownOemTranslator unknownOemTranslator;

    @Override
    public CollectionDto getCollection(Context chassisContext) throws ContextResolvingException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);
        return new CollectionDto(POWER_ZONES, getAsIdSet(chassis.getPowerZones()));
    }

    @Override
    public PowerZoneDto getResource(Context powerZoneContext) throws ContextResolvingException {
        PowerZone powerZone = (PowerZone) traverser.traverse(powerZoneContext);
        return map(powerZone);
    }

    private PowerZoneDto map(PowerZone powerZone) {
        return PowerZoneDto.newBuilder()
            .id(powerZone.getId().toString())
            .name(powerZone.getName())
            .description(powerZone.getDescription())
            .unknownOems(unknownOemTranslator.translateUnknownOemToDtos(powerZone.getService(), powerZone.getUnknownOems()))
            .maxPsusSupported(powerZone.getMaxPsusSupported())
            .presence(powerZone.getPresence())
            .numberOfPsusPresent(powerZone.getNumberOfPsusPresent())
            .powerConsumedWatts(powerZone.getPowerConsumedWatts())
            .powerOutputWatts(powerZone.getPowerOutputWatts())
            .powerCapacityWatts(powerZone.getPowerCapacityWatts())
            .status(powerZone.getStatus())
            .rackLocation(mapRackLocation(powerZone.getRackLocation()))
            .powerSupplies(mapPowerSuppliesCollection(powerZone.getPowerSupplies()))
            .build();
    }

    private List<PowerZonePowerSupplyDto> mapPowerSuppliesCollection(Collection<PowerZonePowerSupply> powerSupplies) {
        List<PowerZonePowerSupplyDto> dtos = new ArrayList<>();

        if (powerSupplies != null) {
            for (PowerZonePowerSupply psu : powerSupplies) {
                dtos.add(mapPowerSupply(psu));
            }
        }

        return dtos;
    }

    private PowerZonePowerSupplyDto mapPowerSupply(PowerZonePowerSupply psu) {
        return PowerZonePowerSupplyDto.newBuilder()
            .name(psu.getName())
            .powerCapacityWatts(psu.getPowerCapacityWatts())
            .lastPowerOutputWatt(psu.getLastPowerOutputWatt())
            .serialNumber(psu.getSerialNumber())
            .manufacturer(psu.getManufacturer())
            .modelNumber(psu.getModelNumber())
            .partNumber(psu.getPartNumber())
            .firmwareRevision(psu.getFirmwareRevision())
            .status(psu.getStatus())
            .rackLocation(mapRackLocation(psu.getRackLocation()))
            .build();
    }

    private RackLocationDto mapRackLocation(RackLocation rackLocation) {
        return rackLocation == null ? null
            : RackLocationDto.newBuilder()
            .rackUnitType(rackLocation.getRackUnit())
            .uHeight(rackLocation.getUHeight())
            .uLocation(rackLocation.getULocation())
            .xLocation(rackLocation.getXLocation())
            .build();
    }
}
