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

package com.intel.podm.mappers.redfish;

import com.intel.podm.business.entities.redfish.PowerZone;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.client.api.resources.redfish.PowerZoneResource;
import com.intel.podm.mappers.EntityMapper;
import com.intel.podm.mappers.subresources.PowerZonePowerSupplyMapper;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;

@Dependent
public class PowerZoneMapper extends EntityMapper<PowerZoneResource, PowerZone> {
    @Inject
    private PowerZonePowerSupplyMapper powerZonePowerSupplyMapper;

    public PowerZoneMapper() {
        super(PowerZoneResource.class, PowerZone.class);
        registerProvider(RackLocation.class, target -> provideRackLocation());
    }

    @Override
    protected void performNotAutomatedMapping(PowerZoneResource sourcePowerZone, PowerZone targetPowerZone) {
        super.performNotAutomatedMapping(source, target);
        powerZonePowerSupplyMapper.map(sourcePowerZone.getPowerSupplies(), targetPowerZone.getPowerSupplies(), targetPowerZone::addPowerSupply);
    }

    private RackLocation provideRackLocation() {
        RackLocation rackLocation = target.getRackLocation();
        return rackLocation == null ? new RackLocation() : rackLocation;
    }
}
