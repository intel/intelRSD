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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.resources.redfish.PowerZonePowerSupplyResource;
import com.intel.podm.client.api.resources.redfish.PowerZoneResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.redfish.properties.PowerZonePowerSupplyResourceImpl;
import com.intel.podm.client.resources.redfish.properties.RackLocationObjectImpl;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.List;

@OdataTypes({
    "#PowerZone" + OdataTypes.VERSION_PATTERN + "PowerZone",
    "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.PowerZone",
    "#Intel\\.Oem\\.RmmPowerZone"
})
public class PowerZoneResourceImpl extends ExternalServiceResourceImpl implements PowerZoneResource {
    @JsonProperty("Presence")
    private String presence;

    @JsonProperty("PowerConsumedWatts")
    private Integer powerConsumedWatts;

    @JsonProperty("PowerOutputWatts")
    private Integer powerOutputWatts;

    @JsonProperty("PowerCapacityWatts")
    private Integer powerCapacityWatts;

    @JsonProperty("Status")
    private Status status;

    @JsonProperty("MaxPSUsSupported")
    private Integer maxPsusSupported;

    @JsonProperty("NumberOfPSUsPresent")
    private Integer numberOfPsusPresent;

    @JsonProperty("RackLocation")
    private RackLocationObjectImpl rackLocation;

    @JsonProperty("PowerSupplies")
    private List<PowerZonePowerSupplyResourceImpl> powerSupplies = new ArrayList<>();

    @Override
    public String getPresence() {
        return presence;
    }

    @Override
    public Integer getPowerConsumedWatts() {
        return powerConsumedWatts;
    }

    @Override
    public Integer getPowerOutputWatts() {
        return powerOutputWatts;
    }

    @Override
    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public Integer getMaxPsusSupported() {
        return maxPsusSupported;
    }

    @Override
    public Integer getNumberOfPsusPresent() {
        return numberOfPsusPresent;
    }

    @Override
    public RackLocationObjectImpl getRackLocation() {
        return rackLocation;
    }

    @Override
    public List<PowerZonePowerSupplyResource> getPowerSupplies() {
        return (List) powerSupplies;
    }
}
