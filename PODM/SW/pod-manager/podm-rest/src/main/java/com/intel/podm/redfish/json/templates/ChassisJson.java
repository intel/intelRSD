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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.ChassisLinksJson;
import com.intel.podm.rest.odataid.ODataId;

import java.util.UUID;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "chassisType", "name",
        "description", "manufacturer", "model", "sku", "serialNumber", "partNumber",
        "assetTag", "indicatorLed", "status", "thermalZones", "powerZones", "oem", "links"
})
public class ChassisJson extends BaseJson {
    public String id;
    public ChassisType chassisType;
    public String name;
    public String description;
    public String manufacturer;
    public String model;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    public String assetTag;
    @JsonProperty("IndicatorLED")
    public IndicatorLed indicatorLed;
    public Status status;
    public ODataId thermalZones;
    public ODataId powerZones;
    public ChassisOem oem;
    public ChassisLinksJson links = new ChassisLinksJson();

    public ChassisJson() {
        super("#Chassis.1.2.0.Chassis");
    }

    public static class ChassisOem {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"oDataType", "location"})
        public static class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType;
            public Location location = new Location();

            public RackScaleOem() {
                this.oDataType = "#Intel.Oem.Chassis";
            }
        }

        @JsonPropertyOrder({"id", "parentId"})
        public static class Location {
            public String id;
            public String parentId;
        }
    }

    public static class RackChassisOem extends ChassisOem {
        @JsonProperty("Intel_RackScale")
        public RackChassisRackScaleOem rackChassisOem = new RackChassisRackScaleOem();

        public static class RackChassisRackScaleOem extends RackScaleOem {
            @JsonProperty("RMMPresent")
            public Boolean rmmPresent;
            public Boolean rackSupportsDisaggregatedPowerCooling;
            @JsonProperty("UUID")
            public UUID uuid;
            public String geoTag;

            public RackChassisRackScaleOem() {
                super();
                this.oDataType = "#Intel.Oem.RackChassis";
            }
        }
    }
}
