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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "chassisType", "name",
    "description", "manufacturer", "model", "sku", "serialNumber", "partNumber",
    "assetTag", "indicatorLed", "status", "thermalZones", "powerZones", "power", "thermal", "oem", "links"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class ChassisJson extends BaseResourceJson {
    public ChassisType chassisType;
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
    @JsonInclude(NON_NULL)
    public ODataId thermal;
    @JsonInclude(NON_NULL)
    public ODataId power;
    public Oem oem;
    public Links links = new Links();
    public Actions actions = new Actions();

    public ChassisJson() {
        super("#Chassis.v1_3_0.Chassis");
    }

    public RackChassisOem createNewRackChassisOem() {
        return new RackChassisOem();
    }

    public Oem createNewChassisOem() {
        return new Oem();
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({"oDataType", "location"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            public String oDataType;
            public Location location = new Location();

            public RackScaleOem() {
                this.oDataType = "#Intel.Oem.Chassis";
            }
        }

        @JsonPropertyOrder({"id", "parentId"})
        public class Location {
            public String id;
            public String parentId;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class RackChassisOem extends Oem {
        @JsonProperty("Intel_RackScale")
        public RackChassisRackScaleOem rackChassisOem = new RackChassisRackScaleOem();

        public class RackChassisRackScaleOem extends RackScaleOem {
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

    @JsonPropertyOrder({"contains", "containedBy", "computerSystems", "switches", "managedBy", "managersInChassis",
        "storage", "drives", "oem"})
    @SuppressWarnings({"checkstyle:VisibilityModifier"})
    public class Links extends RedfishLinksJson {
        @JsonProperty("@odata.type")
        public String oDataType = "#Chassis.v1_2_0.Links";

        public Set<ODataId> contains = new HashSet<>();
        public ODataId containedBy;
        public Set<ODataId> computerSystems = new HashSet<>();
        public Set<ODataId> managedBy = new HashSet<>();
        public Set<ODataId> managersInChassis = new HashSet<>();
        public Collection<ODataId> drives = new ArrayList<>();
        public Set<ODataId> storage = new HashSet<>();
        public Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemJson {
            @JsonProperty("Intel_RackScale")
            public RackScaleOem rackScaleOem = new RackScaleOem();

            @JsonPropertyOrder({"oDataType", "switches"})
            public class RackScaleOem {
                @JsonProperty("@odata.type")
                public String oDataType = "#Intel.Oem.ChassisLinks";
                public Set<ODataId> switches = new HashSet<>();
            }
        }
    }

    public class Actions extends RedfishActionsJson {
    }
}
