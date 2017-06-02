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

package com.intel.podm.mappers.subresources;

import com.intel.podm.business.entities.redfish.PowerZonePowerSupply;
import com.intel.podm.business.entities.redfish.embeddables.RackLocation;
import com.intel.podm.client.api.resources.redfish.PowerZonePowerSupplyResource;
import com.intel.podm.mappers.subresources.strategies.EntityCleanAndCreateStrategy;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Objects;

@Dependent
public class PowerZonePowerSupplyMapper extends SubresourceMapper<PowerZonePowerSupplyResource, PowerZonePowerSupply> {
    @Inject
    private RackLocationMapper rackLocationMapper;

    @Inject
    public PowerZonePowerSupplyMapper(EntityCleanAndCreateStrategy<PowerZonePowerSupply> entityCleanAndCreateStrategy) {
        super(PowerZonePowerSupply.class, entityCleanAndCreateStrategy);
    }

    @Override
    public void map(PowerZonePowerSupplyResource sourcePowerSupply, PowerZonePowerSupply targetPowerZonePowerSupply) {
        targetPowerZonePowerSupply.setName(sourcePowerSupply.getName());
        targetPowerZonePowerSupply.setStatus(sourcePowerSupply.getStatus());
        targetPowerZonePowerSupply.setPowerCapacityWatts(sourcePowerSupply.getPowerCapacityWatts());
        targetPowerZonePowerSupply.setLastPowerOutputWatt(sourcePowerSupply.getLastPowerOutputWatts());
        targetPowerZonePowerSupply.setSerialNumber(sourcePowerSupply.getSerialNumber());
        targetPowerZonePowerSupply.setManufacturer(sourcePowerSupply.getManufacturer());
        targetPowerZonePowerSupply.setModelNumber(sourcePowerSupply.getModelNumber());
        targetPowerZonePowerSupply.setPartNumber(sourcePowerSupply.getPartNumber());
        targetPowerZonePowerSupply.setFirmwareRevision(sourcePowerSupply.getFirmwareVersion());

        if (sourcePowerSupply.getRackLocation() != null) {
            RackLocation rackLocation = new RackLocation();
            rackLocationMapper.map(sourcePowerSupply.getRackLocation(), rackLocation);
            targetPowerZonePowerSupply.setRackLocation(rackLocation);
        }
    }

    @Override
    public boolean equals(PowerZonePowerSupplyResource sourcePowerSupply, PowerZonePowerSupply targetPowerZonePowerSupply) {
        return propertiesEquals(sourcePowerSupply, targetPowerZonePowerSupply)
                && powerAndLocationEquals(sourcePowerSupply, targetPowerZonePowerSupply)
                && fruInfoEquals(sourcePowerSupply, targetPowerZonePowerSupply);
    }

    private boolean propertiesEquals(PowerZonePowerSupplyResource sourcePowerSupply, PowerZonePowerSupply targetPowerZonePowerSupply) {
        return Objects.equals(sourcePowerSupply.getName(), targetPowerZonePowerSupply.getName())
                && Objects.equals(sourcePowerSupply.getStatus(), targetPowerZonePowerSupply.getStatus())
                && Objects.equals(sourcePowerSupply.getPartNumber(), targetPowerZonePowerSupply.getPartNumber())
                && Objects.equals(sourcePowerSupply.getFirmwareVersion(), targetPowerZonePowerSupply.getFirmwareRevision());
    }

    private boolean powerAndLocationEquals(PowerZonePowerSupplyResource sourcePowerSupply, PowerZonePowerSupply targetPowerZonePowerSupply) {
        return Objects.equals(sourcePowerSupply.getPowerCapacityWatts(), targetPowerZonePowerSupply.getPowerCapacityWatts())
                && Objects.equals(sourcePowerSupply.getLastPowerOutputWatts(), targetPowerZonePowerSupply.getLastPowerOutputWatt())
                && rackLocationMapper.equals(sourcePowerSupply.getRackLocation(), targetPowerZonePowerSupply.getRackLocation());
    }

    private boolean fruInfoEquals(PowerZonePowerSupplyResource sourcePowerSupply, PowerZonePowerSupply targetPowerZonePowerSupply) {
        return Objects.equals(sourcePowerSupply.getSerialNumber(), targetPowerZonePowerSupply.getSerialNumber())
                && Objects.equals(sourcePowerSupply.getManufacturer(), targetPowerZonePowerSupply.getManufacturer())
                && Objects.equals(sourcePowerSupply.getModelNumber(), targetPowerZonePowerSupply.getModelNumber());
    }
}
