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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.common.types.Status;

public final class PowerZonePowerSupplyDto {
    private final String name;
    private final Integer powerCapacityWatts;
    private final Integer lastPowerOutputWatt;
    private final String serialNumber;
    private final String manufacturer;
    private final String modelNumber;
    private final String partNumber;
    private final String firmwareRevision;
    private final Status status;
    private final RackLocationDto rackLocation;

    private PowerZonePowerSupplyDto(Builder builder) {
        name = builder.name;
        powerCapacityWatts = builder.powerCapacityWatts;
        lastPowerOutputWatt = builder.lastPowerOutputWatt;
        serialNumber = builder.serialNumber;
        manufacturer = builder.manufacturer;
        modelNumber = builder.modelNumber;
        partNumber = builder.partNumber;
        firmwareRevision = builder.firmwareRevision;
        status = builder.status;
        rackLocation = builder.rackLocation;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getName() {
        return name;
    }

    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public Integer getLastPowerOutputWatt() {
        return lastPowerOutputWatt;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModelNumber() {
        return modelNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getFirmwareRevision() {
        return firmwareRevision;
    }

    public Status getStatus() {
        return status;
    }

    public RackLocationDto getRackLocation() {
        return rackLocation;
    }

    public static final class Builder {
        private String name;
        private Integer powerCapacityWatts;
        private Integer lastPowerOutputWatt;
        private String serialNumber;
        private String manufacturer;
        private String modelNumber;
        private String partNumber;
        private String firmwareRevision;
        private Status status;
        private RackLocationDto rackLocation;

        private Builder() {
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder powerCapacityWatts(Integer powerCapacityWatts) {
            this.powerCapacityWatts = powerCapacityWatts;
            return this;
        }

        public Builder lastPowerOutputWatt(Integer lastPowerOutputWatt) {
            this.lastPowerOutputWatt = lastPowerOutputWatt;
            return this;
        }

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public Builder modelNumber(String modelNumber) {
            this.modelNumber = modelNumber;
            return this;
        }

        public Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public Builder firmwareRevision(String firmwareRevision) {
            this.firmwareRevision = firmwareRevision;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder rackLocation(RackLocationDto rackLocation) {
            this.rackLocation = rackLocation;
            return this;
        }

        public PowerZonePowerSupplyDto build() {
            return new PowerZonePowerSupplyDto(this);
        }
    }
}
