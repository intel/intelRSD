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
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.PowerSupplyJson;
import com.intel.podm.redfish.json.templates.attributes.RackLocationAttributeJson;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status",
        "rackLocation", "maxPsusSupported", "presence", "numberOfPsusPresent", "powerConsumedWatts",
        "powerOutputWatts", "powerCapacityWatts", "powerSupplies", "actions", "links"
})
public class PowerZoneJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
    public Status status;
    public RackLocationAttributeJson rackLocation;
    @JsonProperty("MaxPSUsSupported")
    public Integer maxPsusSupported;
    public String presence;
    @JsonProperty("NumberOfPSUsPresent")
    public Integer numberOfPsusPresent;
    public Integer powerConsumedWatts;
    public Integer powerOutputWatts;
    public Integer powerCapacityWatts;
    public List<PowerSupplyJson> powerSupplies = new ArrayList<>();
    public Object actions = new Object();
    public Object links = new Object();

    public PowerZoneJson() {
        super("#PowerZone.1.0.0.PowerZone");
    }
}
