/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.PowerSupply;
import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.properties.RackLocation;
import com.intel.podm.client.api.resources.redfish.PowerSupplyResource;
import com.intel.podm.client.api.resources.redfish.PowerZoneResource;
import com.intel.podm.client.api.resources.redfish.RackLocationObject;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.function.Supplier;

import static java.util.Objects.isNull;

@Dependent
public class PowerZoneMapper extends DomainObjectMapper<PowerZoneResource, PowerZone> {
    @Inject
    private GenericDao genericDao;

    public PowerZoneMapper() {
        super(PowerZoneResource.class, PowerZone.class);
        registerProvider(RackLocation.class, this::provideRackLocation);
    }

    @Override
    protected void performNotAutomatedMapping(PowerZoneResource source, PowerZone target) {
        clearPsus(target.getPowerSupplies());
        addPsus(source.getPowerSupplies(), target::addPowerSupply);
    }

    private void clearPsus(Collection<PowerSupply> psus) {
        psus.forEach(genericDao::remove);
    }

    private void addPsus(Iterable<PowerSupplyResource> psus, Supplier<PowerSupply> psuSupplier) {
        if (isNull(psus)) {
            return;
        }

        for (PowerSupplyResource psu : psus) {
            PowerSupply powerSupply = psuSupplier.get();
            powerSupply.setName(psu.getName());
            powerSupply.setStatus(psu.getStatus());
            powerSupply.setPowerCapacityWatts(psu.getPowerCapacityWatts());
            powerSupply.setLastPowerOutputWatt(psu.getLastPowerOutputWatts());
            powerSupply.setSerialNumber(psu.getSerialNumber());
            powerSupply.setManufacturer(psu.getManufacturer());
            powerSupply.setModel(psu.getModel());
            powerSupply.setPartNumber(psu.getPartNumber());
            powerSupply.setFirmwareVersion(psu.getFirmwareVersion());
            powerSupply.setRackLocation(toRackLocation(psu.getRackLocation()));
        }
    }

    private RackLocation toRackLocation(RackLocationObject rackLocation) {
        if (isNull(rackLocation)) {
            return null;
        }

        RackLocation location = createRackLocation();
        location.setRackUnit(rackLocation.getRackUnit());
        location.setUHeight(rackLocation.getUHeight());
        location.setULocation(rackLocation.getULocation());
        location.setXLocation(rackLocation.getXLocation());

        return location;
    }

    private RackLocation createRackLocation() {
        return genericDao.create(RackLocation.class);
    }

    private RackLocation provideRackLocation(RackLocationObject rackLocationObject) {
        RackLocation rackLocation = target.getRackLocation();
        if (rackLocation == null) {
            rackLocation = genericDao.create(RackLocation.class);
        }

        return rackLocation;
    }
}
