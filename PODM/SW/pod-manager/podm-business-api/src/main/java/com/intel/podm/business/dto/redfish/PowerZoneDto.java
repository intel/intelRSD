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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.PowerSupplyDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

import java.util.List;

public final class PowerZoneDto {
    private final Id id;
    private final String name;
    private final String description;
    private final Status status;
    private final RackLocationDto rackLocation;
    private final Integer maxPsusSupported;
    private final String presence;
    private final Integer numberOfPsusPresent;
    private final Integer powerConsumedWatts;
    private final Integer powerCapacityWatts;
    private final Integer powerOutputWatts;
    private final List<PowerSupplyDto> powerSupplies;

    private PowerZoneDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        status = builder.status;
        rackLocation = builder.rackLocation;
        maxPsusSupported = builder.maxPsusSupported;
        presence = builder.presence;
        numberOfPsusPresent = builder.numberOfPsusPresent;
        powerConsumedWatts = builder.powerConsumedWatts;
        powerCapacityWatts = builder.powerCapacityWatts;
        powerOutputWatts = builder.powerOutputWatts;
        powerSupplies = builder.powerSupplies;
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

    public String getDescription() {
        return description;
    }

    public Status getStatus() {
        return status;
    }

    public RackLocationDto getRackLocation() {
        return rackLocation;
    }

    public Integer getMaxPsusSupported() {
        return maxPsusSupported;
    }

    public String getPresence() {
        return presence;
    }

    public Integer getNumberOfPsusPresent() {
        return numberOfPsusPresent;
    }

    public Integer getPowerConsumedWatts() {
        return powerConsumedWatts;
    }

    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public Integer getPowerOutputWatts() {
        return powerOutputWatts;
    }

    public List<PowerSupplyDto> getPowerSupplies() {
        return powerSupplies;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
        private Status status;
        private RackLocationDto rackLocation;
        private Integer maxPsusSupported;
        private String presence;
        private Integer numberOfPsusPresent;
        private Integer powerConsumedWatts;
        private Integer powerCapacityWatts;
        private Integer powerOutputWatts;
        private List<PowerSupplyDto> powerSupplies;

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

        public Builder description(String val) {
            description = val;
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

        public Builder maxPsusSupported(Integer val) {
            maxPsusSupported = val;
            return this;
        }

        public Builder presence(String val) {
            presence = val;
            return this;
        }

        public Builder numberOfPsusPresent(Integer val) {
            numberOfPsusPresent = val;
            return this;
        }

        public Builder powerConsumedWatts(Integer val) {
            powerConsumedWatts = val;
            return this;
        }

        public Builder powerCapacityWatts(Integer val) {
            powerCapacityWatts = val;
            return this;
        }

        public Builder powerOutputWatts(Integer val) {
            powerOutputWatts = val;
            return this;
        }

        public Builder powerSupplies(List<PowerSupplyDto> val) {
            powerSupplies = val;
            return this;
        }

        public PowerZoneDto build() {
            return new PowerZoneDto(this);
        }
    }
}
