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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.Status;

import java.util.Collection;
import java.util.UUID;

public final class ChassisDto {
    private final String id;
    private final ChassisType chassisType;
    private final String name;
    private final String description;
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
    private final String powerState;

    /** RMM Atributes */
    private final UUID uuid;
    private final String geoTag;
    private final Boolean rackSupportsDisaggregatedPowerCooling;
    private final Integer rackPuid;
    private final Boolean rmmPresent;

    private final Collection<Context> contains;
    private final Context containedBy;
    private final Collection<Context> computerSystems;
    private final Collection<Context> switches;
    private final Collection<Context> managedBy;
    private final Collection<Context> managersInChassis;


    private ChassisDto(Builder builder) {
        id = builder.id;
        chassisType = builder.chassisType;
        name = builder.name;
        description = builder.description;
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
        powerState = builder.powerState;
        uuid = builder.uuid;
        geoTag = builder.geoTag;
        rackSupportsDisaggregatedPowerCooling = builder.rackSupportsDisaggregatedPowerCooling;
        rackPuid = builder.rackPuid;
        contains = builder.contains;
        containedBy = builder.containedBy;
        computerSystems = builder.computerSystems;
        switches = builder.switches;
        managedBy = builder.managedBy;
        managersInChassis = builder.managersInChassis;
        rmmPresent = builder.rmmPresent;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getId() {
        return id;
    }

    public ChassisType getChassisType() {
        return chassisType;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
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

    public String getPowerState() {
        return powerState;
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

    public Integer getRackPuid() {
        return rackPuid;
    }

    public Collection<Context> getContains() {
        return contains;
    }

    public Context getContainedBy() {
        return containedBy;
    }

    public Collection<Context> getComputerSystems() {
        return computerSystems;
    }

    public Collection<Context> getSwitches() {
        return switches;
    }

    public Collection<Context> getManagedBy() {
        return managedBy;
    }
    public Collection<Context> getManagersInChassis() {
            return managersInChassis;
        }

    public Boolean getRmmPresent() {
        return rmmPresent;
    }

    public static final class Builder {
        private String id;
        private ChassisType chassisType;
        private String name;
        private String description;
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
        private String powerState;
        private UUID uuid;
        private String geoTag;
        private Boolean rackSupportsDisaggregatedPowerCooling;
        private Integer rackPuid;
        private Collection<Context> contains;
        private Context containedBy;
        private Collection<Context> computerSystems;
        private Collection<Context> switches;
        private Collection<Context> managedBy;
        private Collection<Context> managersInChassis;
        private Boolean rmmPresent;

        private Builder() {
        }

        public Builder id(String val) {
            id = val;
            return this;
        }

        public Builder chassisType(ChassisType val) {
            chassisType = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder manufacturer(String val) {
            manufacturer = val;
            return this;
        }

        public Builder model(String val) {
            model = val;
            return this;
        }

        public Builder sku(String val) {
            sku = val;
            return this;
        }

        public Builder serialNumber(String val) {
            serialNumber = val;
            return this;
        }

        public Builder partNumber(String val) {
            partNumber = val;
            return this;
        }

        public Builder assetTag(String val) {
            assetTag = val;
            return this;
        }

        public Builder indicatorLed(IndicatorLed val) {
            indicatorLed = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder locationId(String val) {
            locationId = val;
            return this;
        }

        public Builder locationParentId(String val) {
            locationParentId = val;
            return this;
        }

        public Builder powerState(String val) {
            powerState = val;
            return this;
        }

        public Builder uuid(UUID val) {
            uuid = val;
            return this;
        }

        public Builder geoTag(String val) {
            geoTag = val;
            return this;
        }

        public Builder rackSupportsDisaggregatedPowerCooling(Boolean val) {
            rackSupportsDisaggregatedPowerCooling = val;
            return this;
        }

        public Builder rackPuid(Integer val) {
            rackPuid = val;
            return this;
        }

        public Builder contains(Collection<Context> val) {
            contains = val;
            return this;
        }

        public Builder containedBy(Context val) {
            containedBy = val;
            return this;
        }

        public Builder computerSystems(Collection<Context> val) {
            computerSystems = val;
            return this;
        }

        public Builder switches(Collection<Context> val) {
            switches = val;
            return this;
        }

        public Builder managedBy(Collection<Context> val) {
            managedBy = val;
            return this;
        }

        public Builder managersInChassis(Collection<Context> val) {
            managersInChassis = val;
            return this;
        }

        public Builder rmmPresent(Boolean val) {
            rmmPresent = val;
            return this;
        }

        public ChassisDto build() {
            return new ChassisDto(this);
        }
    }
}
