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
import com.intel.podm.client.api.resources.redfish.FanResource;
import com.intel.podm.client.api.resources.redfish.RackLocationObject;
import com.intel.podm.client.api.resources.redfish.TemperatureResource;
import com.intel.podm.client.api.resources.redfish.ThermalZoneResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.redfish.properties.FanResourceImpl;
import com.intel.podm.client.resources.redfish.properties.RackLocationObjectImpl;
import com.intel.podm.client.resources.redfish.properties.TemperatureResourceImpl;
import com.intel.podm.common.types.Status;

import java.util.ArrayList;
import java.util.List;
import java.util.UUID;

@OdataTypes({
    "#ThermalZone" + OdataTypes.VERSION_PATTERN + "ThermalZone",
    "http://rackscale\\.intel\\.com/schema#Intel\\.Oem\\.ThermalZone",
    "#Intel\\.Oem\\.RmmThermalZone"
})
public class ThermalZoneResourceImpl extends ExternalServiceResourceImpl implements ThermalZoneResource {
    @JsonProperty("UUID")
    private UUID uuid;
    @JsonProperty("RackLocation")
    private RackLocationObjectImpl rackLocationObject;
    @JsonProperty("Presence")
    private String presence;
    @JsonProperty("DesiredSpeedPWM")
    private Integer desiredSpeedPwm;
    @JsonProperty("DesiredSpeedRPM")
    private Integer desiredSpeedRpm;
    @JsonProperty("MaxFansSupported")
    private Integer maxFansSupported;
    @JsonProperty("NumberOfFansPresent")
    private Integer numberOfFansPresent;
    @JsonProperty("VolumetricAirflow")
    private Integer volumetricAirflow;
    @JsonProperty("Temperatures")
    private List<TemperatureResourceImpl> temperatures = new ArrayList<>();
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Fans")
    private List<FanResourceImpl> fans = new ArrayList<>();

    @Override
    public String getPresence() {
        return presence;
    }

    @Override
    public Integer getDesiredSpeedPwm() {
        return desiredSpeedPwm;
    }

    @Override
    public Integer getDesiredSpeedRpm() {
        return desiredSpeedRpm;
    }

    @Override
    public Integer getMaxFansSupported() {
        return maxFansSupported;
    }

    @Override
    public Integer getNumberOfFansPresent() {
        return numberOfFansPresent;
    }

    @Override
    public Integer getVolumetricAirflow() {
        return volumetricAirflow;
    }

    @Override
    public RackLocationObject getRackLocation() {
        return rackLocationObject;
    }

    @Override
    public List<TemperatureResource> getTemperatures() {
        return (List) temperatures;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public List<FanResource> getFans() {
        return (List) fans;
    }
}
