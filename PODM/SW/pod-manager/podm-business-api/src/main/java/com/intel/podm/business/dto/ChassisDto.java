/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "chassisType", "name", "description", "manufacturer", "model", "sku", "serialNumber", "partNumber",
    "assetTag", "indicatorLed", "powerState", "status", "power", "thermal", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ChassisDto extends RedfishDto {
    private final Oem oem = new Oem();
    private final Links links = new Links();
    private final Actions actions = new Actions();
    private ChassisType chassisType;
    private String manufacturer;
    private String model;
    @JsonProperty("SKU")
    private String sku;
    private String serialNumber;
    private String partNumber;
    private String assetTag;
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    private PowerState powerState;
    private Status status;
    @JsonInclude(NON_NULL)
    private SingletonContext thermal;
    @JsonInclude(NON_NULL)
    private SingletonContext power;

    public ChassisDto() {
        super("#Chassis.v1_3_0.Chassis");
    }

    public ChassisType getChassisType() {
        return chassisType;
    }

    public void setChassisType(ChassisType chassisType) {
        this.chassisType = chassisType;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public SingletonContext getThermal() {
        return thermal;
    }

    public void setThermal(SingletonContext thermal) {
        this.thermal = thermal;
    }

    public SingletonContext getPower() {
        return power;
    }

    public void setPower(SingletonContext power) {
        this.power = power;
    }

    public Oem getOem() {
        return oem;
    }

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    @JsonPropertyOrder({"rmmPresent", "rackSupportsDisaggregatedPowerCooling", "uuid", "geoTag"})
    public static class RackChassisRackScaleOem {
        @JsonProperty("RMMPresent")
        private Boolean rmmPresent;
        private Boolean rackSupportsDisaggregatedPowerCooling;
        @JsonProperty("UUID")
        private UUID uuid;
        private String geoTag;

        public Boolean getRmmPresent() {
            return rmmPresent;
        }

        public void setRmmPresent(Boolean rmmPresent) {
            this.rmmPresent = rmmPresent;
        }

        public Boolean getRackSupportsDisaggregatedPowerCooling() {
            return rackSupportsDisaggregatedPowerCooling;
        }

        public void setRackSupportsDisaggregatedPowerCooling(Boolean rackSupportsDisaggregatedPowerCooling) {
            this.rackSupportsDisaggregatedPowerCooling = rackSupportsDisaggregatedPowerCooling;
        }

        public UUID getUuid() {
            return uuid;
        }

        public void setUuid(UUID uuid) {
            this.uuid = uuid;
        }

        public String getGeoTag() {
            return geoTag;
        }

        public void setGeoTag(String geoTag) {
            this.geoTag = geoTag;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "location", "rmmPresent", "rackSupportsDisaggregatedPowerCooling", "uuid", "geoTag"})
        public class RackScaleOem {
            @JsonIgnore
            public static final String ODATA_TYPE_DEFAULT = "#Intel.Oem.Chassis";
            @JsonIgnore
            public static final String ODATA_TYPE_RACK = "#Intel.Oem.RackChassis";

            @JsonProperty("@odata.type")
            private String oDataType = ODATA_TYPE_DEFAULT;
            @JsonUnwrapped
            @JsonInclude(NON_NULL)
            private RackChassisRackScaleOem rackChassisOem;

            private Location location = new Location();

            public String getoDataType() {
                return oDataType;
            }

            public void setoDataType(String oDataType) {
                this.oDataType = oDataType;
            }

            public RackChassisRackScaleOem getRackChassisOem() {
                return rackChassisOem;
            }

            public void setRackChassisOem(RackChassisRackScaleOem rackChassisOem) {
                this.rackChassisOem = rackChassisOem;
            }

            public Location getLocation() {
                return location;
            }

            public void setLocation(Location location) {
                this.location = location;
            }
        }

        @JsonPropertyOrder({"id", "parentId"})
        public class Location {
            private String id;
            private String parentId;

            public String getId() {
                return id;
            }

            public void setId(String id) {
                this.id = id;
            }

            public String getParentId() {
                return parentId;
            }

            public void setParentId(String parentId) {
                this.parentId = parentId;
            }
        }
    }

    @JsonPropertyOrder({
        "@odata.type", "contains", "containedBy", "computerSystems", "managedBy", "managersInChassis", "storage", "cooledBy", "poweredBy", "drives", "oem"
    })
    public final class Links extends RedfishLinksDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#Chassis.v1_2_0.Links";
        private final Oem oem = new Oem();
        private Set<Context> contains = new HashSet<>();
        private Context containedBy;
        private Set<Context> computerSystems = new HashSet<>();
        private Set<Context> managedBy = new HashSet<>();
        private Set<Context> managersInChassis = new HashSet<>();
        private Set<Context> drives = new HashSet<>();
        private Set<Context> storage = new HashSet<>();
        private Set<Context> poweredBy = new HashSet<>();
        private Set<Context> cooledBy = new HashSet<>();

        public Set<Context> getContains() {
            return contains;
        }

        public void setContains(Set<Context> contains) {
            this.contains = contains;
        }

        public Context getContainedBy() {
            return containedBy;
        }

        public void setContainedBy(Context containedBy) {
            this.containedBy = containedBy;
        }

        public Set<Context> getComputerSystems() {
            return computerSystems;
        }

        public void setComputerSystems(Set<Context> computerSystems) {
            this.computerSystems = computerSystems;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
        }

        public Set<Context> getManagersInChassis() {
            return managersInChassis;
        }

        public void setManagersInChassis(Set<Context> managersInChassis) {
            this.managersInChassis = managersInChassis;
        }

        public Set<Context> getDrives() {
            return drives;
        }

        public void setDrives(Set<Context> drives) {
            this.drives = drives;
        }

        public Set<Context> getPoweredBy() {
            return poweredBy;
        }

        public void setPoweredBy(Set<Context> poweredBy) {
            this.poweredBy = poweredBy;
        }

        public Set<Context> getCooledBy() {
            return cooledBy;
        }

        public void setCooledBy(Set<Context> cooledBy) {
            this.cooledBy = cooledBy;
        }

        public Set<Context> getStorage() {
            return storage;
        }

        public void setStorage(Set<Context> storage) {
            this.storage = storage;
        }

        public Oem getOem() {
            return oem;
        }

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemDto {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonPropertyOrder({"@odata.type", "switches"})
            public class RackScaleOem {
                @JsonProperty("@odata.type")
                private final String oDataType = "#Intel.Oem.ChassisLinks";
                private Set<Context> switches = new HashSet<>();

                public Set<Context> getSwitches() {
                    return switches;
                }

                public void setSwitches(Set<Context> switches) {
                    this.switches = switches;
                }
            }
        }
    }

    public class Actions extends RedfishActionsDto {
    }
}
