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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.PowerZonePowerSupplyDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerZoneDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final RackLocationDto rackLocation;
    private final Integer maxPsusSupported;
    private final String presence;
    private final Integer numberOfPsusPresent;
    private final Integer powerConsumedWatts;
    private final Integer powerCapacityWatts;
    private final Integer powerOutputWatts;
    private final List<PowerZonePowerSupplyDto> powerSupplies;

    private PowerZoneDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
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

    public List<PowerZonePowerSupplyDto> getPowerSupplies() {
        return powerSupplies;
    }

    @Override
    public Links getLinks() {
        throw new UnsupportedOperationException("Links are not available in this resource");
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private RackLocationDto rackLocation;
        private Integer maxPsusSupported;
        private String presence;
        private Integer numberOfPsusPresent;
        private Integer powerConsumedWatts;
        private Integer powerCapacityWatts;
        private Integer powerOutputWatts;
        private List<PowerZonePowerSupplyDto> powerSupplies;

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

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder rackLocation(RackLocationDto rackLocation) {
            this.rackLocation = rackLocation;
            return this;
        }

        public Builder maxPsusSupported(Integer maxPsusSupported) {
            this.maxPsusSupported = maxPsusSupported;
            return this;
        }

        public Builder presence(String presence) {
            this.presence = presence;
            return this;
        }

        public Builder numberOfPsusPresent(Integer numberOfPsusPresent) {
            this.numberOfPsusPresent = numberOfPsusPresent;
            return this;
        }

        public Builder powerConsumedWatts(Integer powerConsumedWatts) {
            this.powerConsumedWatts = powerConsumedWatts;
            return this;
        }

        public Builder powerCapacityWatts(Integer powerCapacityWatts) {
            this.powerCapacityWatts = powerCapacityWatts;
            return this;
        }

        public Builder powerOutputWatts(Integer powerOutputWatts) {
            this.powerOutputWatts = powerOutputWatts;
            return this;
        }

        public Builder powerSupplies(List<PowerZonePowerSupplyDto> powerSupplies) {
            this.powerSupplies = powerSupplies;
            return this;
        }

        public PowerZoneDto build() {
            return new PowerZoneDto(this);
        }
    }
}
