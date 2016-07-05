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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.CollectionDto;
import com.intel.podm.business.dto.redfish.PowerZoneDto;
import com.intel.podm.business.dto.redfish.attributes.PowerSupplyDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.properties.RackLocation;
import com.intel.podm.business.redfish.Contexts;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.PowerZoneService;

import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.business.dto.redfish.CollectionDto.Type.POWER_ZONES;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Transactional(REQUIRED)
public class PowerZoneServiceImpl implements PowerZoneService {
    @Inject
    private DomainObjectTreeTraverser traverser;

    @Override
    public CollectionDto getPowerZonesCollection(Context chassisContext) throws EntityNotFoundException {
        Chassis chassis = (Chassis) traverser.traverse(chassisContext);
        return new CollectionDto(POWER_ZONES, Contexts.getAsIdList(chassis.getPowerZones()));
    }

    @Override
    public PowerZoneDto getPowerZone(Context powerZoneContext) throws EntityNotFoundException {
        PowerZone powerZone = (PowerZone) traverser.traverse(powerZoneContext);
        return map(powerZoneContext, powerZone);
    }

    private PowerZoneDto map(Context context, PowerZone powerZone) {
        return PowerZoneDto.newBuilder()
                .id(powerZone.getId())
                .name(powerZone.getName())
                .description(powerZone.getDescription())
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

    private List<PowerSupplyDto> mapPowerSuppliesCollection(List<PowerSupply> powerSupplies) {
        List<PowerSupplyDto> dtos = new ArrayList<>();

        if (powerSupplies != null) {
            for (PowerSupply psu : powerSupplies) {
                dtos.add(mapPowerSupply(psu));
            }
        }

        return dtos;
    }

    private PowerSupplyDto mapPowerSupply(PowerSupply psu) {
        return PowerSupplyDto.newBuilder()
                .id(psu.getId())
                .name(psu.getName())
                .powerCapacityWatts(psu.getPowerCapacityWatts())
                .lastPowerOutputWatt(psu.getLastPowerOutputWatt())
                .serialNumber(psu.getSerialNumber())
                .manufacturer(psu.getManufacturer())
                .model(psu.getModel())
                .partNumber(psu.getPartNumber())
                .firmwareVersion(psu.getFirmwareVersion())
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
