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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ThermalZone;
import com.intel.podm.business.entities.redfish.properties.Fan;
import com.intel.podm.business.entities.redfish.properties.RackLocation;
import com.intel.podm.business.entities.redfish.properties.Temperature;
import com.intel.podm.client.api.resources.redfish.FanResource;
import com.intel.podm.client.api.resources.redfish.RackLocationObject;
import com.intel.podm.client.api.resources.redfish.TemperatureResource;
import com.intel.podm.client.api.resources.redfish.ThermalZoneResource;
import com.intel.podm.mappers.DomainObjectMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.function.Supplier;

import static java.util.Objects.isNull;
import static java.util.Objects.nonNull;

@Dependent
public class ThermalZoneMapper extends DomainObjectMapper<ThermalZoneResource, ThermalZone> {

    @Inject
    private GenericDao genericDao;

    public ThermalZoneMapper() {
        super(ThermalZoneResource.class, ThermalZone.class);
        registerProvider(RackLocation.class, this::provideRackLocation);
    }

    @Override
    protected void performNotAutomatedMapping(ThermalZoneResource source, ThermalZone target) {
        removeDomainObjects(target::getFans);
        removeDomainObjects(target::getTemperatures);

        mapTemperatures(source, target);
        mapFans(source, target);
    }

    private void removeDomainObjects(Supplier<Collection<? extends DomainObject>> supplier) {
        supplier.get().forEach(genericDao::remove);
    }

    private void mapTemperatures(ThermalZoneResource source, ThermalZone target) {
        for (TemperatureResource temperatureResource : source.getTemperatures()) {
            Temperature temperature = genericDao.create(Temperature.class);

            temperature.setName(temperatureResource.getName());
            temperature.setReadingCelsius(temperatureResource.getReadingCelsius());
            temperature.setStatus(temperatureResource.getStatus());
            temperature.setPhysicalContext(temperatureResource.getPhysicalContext());

            target.link(temperature);
        }
    }

    private void mapFans(ThermalZoneResource source, ThermalZone target) {
        for (FanResource fanResource : source.getFans()) {
            Fan fan = genericDao.create(Fan.class);

            fan.setStatus(fanResource.getStatus());
            fan.setName(fanResource.getFanName());
            fan.setReadingRpm(fanResource.getReadingRpm());
            RackLocation rackLocation = toRackLocation(fanResource.getRackLocation());
            if (nonNull(rackLocation)) {
                fan.setRackLocation(rackLocation);
            }

            target.link(fan);
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

    private RackLocation provideRackLocation(RackLocationObject rackLocationObject) {
        RackLocation rackLocation = target.getRackLocation();
        return isNull(rackLocation) ? createRackLocation() : rackLocation;
    }

    private RackLocation createRackLocation() {
        return genericDao.create(RackLocation.class);
    }
}
