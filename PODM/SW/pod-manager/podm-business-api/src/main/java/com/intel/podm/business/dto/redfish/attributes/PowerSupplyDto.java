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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

public final class PowerSupplyDto {
    private final Id id;
    private final String name;
    private final Integer powerCapacityWatts;
    private final Integer lastPowerOutputWatt;
    private final String serialNumber;
    private final String manufacturer;
    private final String model;
    private final String partNumber;
    private final String firmwareVersion;
    private final Status status;
    private final RackLocationDto rackLocation;

    private PowerSupplyDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        powerCapacityWatts = builder.powerCapacityWatts;
        lastPowerOutputWatt = builder.lastPowerOutputWatt;
        serialNumber = builder.serialNumber;
        manufacturer = builder.manufacturer;
        model = builder.model;
        partNumber = builder.partNumber;
        firmwareVersion = builder.firmwareVersion;
        status = builder.status;
        rackLocation = builder.rackLocation;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
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

    public String getModel() {
        return model;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public Status getStatus() {
        return status;
    }

    public RackLocationDto getRackLocation() {
        return rackLocation;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private Integer powerCapacityWatts;
        private Integer lastPowerOutputWatt;
        private String serialNumber;
        private String manufacturer;
        private String model;
        private String partNumber;
        private String firmwareVersion;
        private Status status;
        private RackLocationDto rackLocation;

        private Builder() {
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder powerCapacityWatts(Integer val) {
            powerCapacityWatts = val;
            return this;
        }

        public Builder lastPowerOutputWatt(Integer val) {
            lastPowerOutputWatt = val;
            return this;
        }

        public Builder serialNumber(String val) {
            serialNumber = val;
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

        public Builder partNumber(String val) {
            partNumber = val;
            return this;
        }

        public Builder firmwareVersion(String val) {
            firmwareVersion = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder rackLocation(RackLocationDto val) {
            rackLocation = val;
            return this;
        }

        public PowerSupplyDto build() {
            return new PowerSupplyDto(this);
        }
    }
}
