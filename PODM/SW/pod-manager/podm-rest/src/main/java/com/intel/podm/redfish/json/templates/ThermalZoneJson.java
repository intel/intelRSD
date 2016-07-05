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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.RackLocationAttributeJson;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "rackLocation",
        "presence", "desiredSpeedPwm", "desiredSpeedRpm", "maxFansSupported", "numberOfFansPresent",
        "volumetricAirflow", "temperatures", "status", "fans", "actions", "links"
})
public class ThermalZoneJson extends BaseJson {
    public Id id;
    public String name;
    public RackLocationAttributeJson rackLocation;
    public String description;
    public String presence;
    public Integer maxFansSupported;
    @JsonProperty("DesiredSpeedPWM")
    public Integer desiredSpeedPwm;
    @JsonProperty("DesiredSpeedRPM")
    public Integer desiredSpeedRpm;
    public Integer numberOfFansPresent;
    public Integer volumetricAirflow;
    public List<Temperature> temperatures = new ArrayList<>();
    public Status status;
    public List<Fan> fans = new ArrayList<>();
    public Object actions = new Object();
    public Object links = new Object();


    public ThermalZoneJson() {
        super("#ThermalZone.1.0.0.ThermalZone");
    }

    @JsonPropertyOrder({
            "odataId", "memberId", "name",
            "status", "reading", "readingUnits", "relatedItem", "oem"
    })
    public static class Fan {
        public String name;
        public Status status;
        @JsonProperty("ReadingRPM")
        public Integer readingRpm;
        public RackLocationAttributeJson rackLocation;
    }

    @JsonPropertyOrder({
            "odataId", "memberId", "name", "sensorNumber", "status",
            "readingCelsius", "physicalContext", "relatedItem"
    })
    public static class Temperature {
        public String name;
        public Status status;
        public Integer readingCelsius;
        public PhysicalContext physicalContext;
    }
}
