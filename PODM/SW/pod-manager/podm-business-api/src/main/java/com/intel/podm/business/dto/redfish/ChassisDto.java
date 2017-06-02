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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;
import java.util.UUID;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:MethodLength", "checkstyle:ExecutableStatementCount"})
public final class ChassisDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final ChassisType chassisType;
    private final String manufacturer;
    private final String model;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final String assetTag;
    private final IndicatorLed indicatorLed;
    private final Status status;
    private final String locationId;
    private final String locationParentId;
    private final ThermalDto thermalDto;
    private final PowerDto powerDto;

    /**
     * RMM Atributes
     */
    private final UUID uuid;
    private final String geoTag;
    private final Boolean rackSupportsDisaggregatedPowerCooling;
    private final Boolean rmmPresent;

    private final Links links;

    private ChassisDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        chassisType = builder.chassisType;
        manufacturer = builder.manufacturer;
        model = builder.model;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        assetTag = builder.assetTag;
        indicatorLed = builder.indicatorLed;
        status = builder.status;
        locationId = builder.locationId;
        locationParentId = builder.locationParentId;
        uuid = builder.uuid;
        geoTag = builder.geoTag;
        thermalDto = builder.thermal;
        powerDto = builder.power;
        rackSupportsDisaggregatedPowerCooling = builder.rackSupportsDisaggregatedPowerCooling;
        rmmPresent = builder.rmmPresent;
        links = Links.newBuilder()
            .contains(builder.contains)
            .containedBy(builder.containedBy)
            .computerSystems(builder.computerSystems)
            .switches(builder.switches)
            .managedBy(builder.managedBy)
            .managersInChassis(builder.managersInChassis)
            .drives(builder.drives)
            .storage(builder.storage)
            .build();
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public ChassisType getChassisType() {
        return chassisType;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public String getSku() {
        return sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public Status getStatus() {
        return status;
    }

    public String getLocationId() {
        return locationId;
    }

    public String getLocationParentId() {
        return locationParentId;
    }

    public UUID getUuid() {
        return uuid;
    }

    public String getGeoTag() {
        return geoTag;
    }

    public Boolean getRackSupportsDisaggregatedPowerCooling() {
        return rackSupportsDisaggregatedPowerCooling;
    }

    public Boolean getRmmPresent() {
        return rmmPresent;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public ThermalDto getThermalDto() {
        return thermalDto;
    }

    public PowerDto getPowerDto() {
        return powerDto;
    }

    public static final class Links implements RedfishResource.Links {
        private Set<Context> contains;
        private Context containedBy;
        private Set<Context> computerSystems;
        private Set<Context> switches;
        private Set<Context> managedBy;
        private Set<Context> managersInChassis;
        private Set<Context> drives;
        private Set<Context> storage;

        private Links(Builder builder) {
            contains = builder.contains;
            containedBy = builder.containedBy;
            computerSystems = builder.computerSystems;
            switches = builder.switches;
            managedBy = builder.managedBy;
            managersInChassis = builder.managersInChassis;
            drives = builder.drives;
            storage = builder.storage;
        }

        public Set<Context> getContains() {
            return contains;
        }

        public Context getContainedBy() {
            return containedBy;
        }

        public Set<Context> getComputerSystems() {
            return computerSystems;
        }

        public Set<Context> getSwitches() {
            return switches;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public Set<Context> getManagersInChassis() {
            return managersInChassis;
        }

        public Set<Context> getDrives() {
            return drives;
        }

        public Set<Context> getStorage() {
            return storage;
        }

        public static Builder newBuilder() {
            return new Builder();
        }

        public static final class Builder {
            private Set<Context> contains;
            private Context containedBy;
            private Set<Context> computerSystems;
            private Set<Context> switches;
            private Set<Context> managedBy;
            private Set<Context> managersInChassis;
            private Set<Context> drives;
            private Set<Context> storage;

            private Builder() {
            }

            public Builder contains(Set<Context> contains) {
                this.contains = contains;
                return this;
            }

            public Builder containedBy(Context containedBy) {
                this.containedBy = containedBy;
                return this;
            }

            public Builder computerSystems(Set<Context> computerSystems) {
                this.computerSystems = computerSystems;
                return this;
            }

            public Builder switches(Set<Context> switches) {
                this.switches = switches;
                return this;
            }

            public Builder managedBy(Set<Context> managedBy) {
                this.managedBy = managedBy;
                return this;
            }

            public Builder managersInChassis(Set<Context> managersInChassis) {
                this.managersInChassis = managersInChassis;
                return this;
            }

            public Builder drives(Set<Context> drives) {
                this.drives = drives;
                return this;
            }

            public Builder storage(Set<Context> storage) {
                this.storage = storage;
                return this;
            }

            public Links build() {
                return new Links(this);
            }
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private ChassisType chassisType;
        private String manufacturer;
        private String model;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private String assetTag;
        private IndicatorLed indicatorLed;
        private Status status;
        private String locationId;
        private String locationParentId;
        private UUID uuid;
        private String geoTag;
        private Boolean rackSupportsDisaggregatedPowerCooling;
        private Set<Context> contains;
        private ThermalDto thermal;
        private PowerDto power;
        private Context containedBy;
        private Set<Context> computerSystems;
        private Set<Context> switches;
        private Set<Context> managedBy;
        private Set<Context> managersInChassis;
        private Set<Context> drives;
        private Set<Context> storage;
        private Boolean rmmPresent;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder chassisType(ChassisType chassisType) {
            this.chassisType = chassisType;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder sku(String sku) {
            this.sku = sku;
            return this;
        }

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
            return this;
        }

        public Builder indicatorLed(IndicatorLed indicatorLed) {
            this.indicatorLed = indicatorLed;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder locationId(String locationId) {
            this.locationId = locationId;
            return this;
        }

        public Builder locationParentId(String locationParentId) {
            this.locationParentId = locationParentId;
            return this;
        }

        public Builder uuid(UUID uuid) {
            this.uuid = uuid;
            return this;
        }

        public Builder geoTag(String geoTag) {
            this.geoTag = geoTag;
            return this;
        }

        public Builder rackSupportsDisaggregatedPowerCooling(Boolean rackSupportsDisaggregatedPowerCooling) {
            this.rackSupportsDisaggregatedPowerCooling = rackSupportsDisaggregatedPowerCooling;
            return this;
        }

        public Builder contains(Set<Context> contains) {
            this.contains = contains;
            return this;
        }

        public Builder containedBy(Context containedBy) {
            this.containedBy = containedBy;
            return this;
        }

        public Builder computerSystems(Set<Context> computerSystems) {
            this.computerSystems = computerSystems;
            return this;
        }

        public Builder switches(Set<Context> switches) {
            this.switches = switches;
            return this;
        }

        public Builder managedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
            return this;
        }

        public Builder managersInChassis(Set<Context> managersInChassis) {
            this.managersInChassis = managersInChassis;
            return this;
        }

        public Builder rmmPresent(Boolean rmmPresent) {
            this.rmmPresent = rmmPresent;
            return this;
        }

        public Builder drives(Set<Context> drives) {
            this.drives = drives;
            return this;
        }

        public Builder storage(Set<Context> storage) {
            this.storage = storage;
            return this;
        }

        public Builder thermal(ThermalDto thermal) {
            this.thermal = thermal;
            return this;
        }

        public Builder power(PowerDto power) {
            this.power = power;
            return this;
        }

        public ChassisDto build() {
            return new ChassisDto(this);
        }
    }
}
