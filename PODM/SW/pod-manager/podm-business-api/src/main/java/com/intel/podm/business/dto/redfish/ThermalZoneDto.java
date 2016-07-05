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

import com.intel.podm.business.dto.redfish.attributes.FanDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.dto.redfish.attributes.TemperatureDto;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

import java.util.List;

public final class ThermalZoneDto {
    private final Id id;
    private final String name;
    private final List<TemperatureDto> temperatures;
    private final List<FanDto> fans;
    private final RackLocationDto rackLocation;
    private final Integer maxFansSupported;
    private final Integer desiredSpeedPwm;
    private final Integer desiredSpeedRpm;
    private final String description;
    private final Status status;
    private final String presence;
    private final Integer volumetricAirflow;
    private final Integer numberOfFansPresent;
    private final RackLocationDto rackLocationDto;

    private ThermalZoneDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        temperatures = builder.temperatures;
        fans = builder.fans;
        rackLocation = builder.rackLocation;
        maxFansSupported = builder.maxFansSupported;
        desiredSpeedPwm = builder.desiredSpeedPwm;
        desiredSpeedRpm = builder.desiredSpeedRpm;
        description = builder.description;
        status = builder.status;
        presence = builder.presence;
        volumetricAirflow = builder.volumetricAirflow;
        numberOfFansPresent = builder.numberOfFansPresent;
        rackLocationDto = builder.rackLocationDto;
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

    public List<TemperatureDto> getTemperatures() {
        return temperatures;
    }

    public List<FanDto> getFans() {
        return fans;
    }

    public RackLocationDto getRackLocation() {
        return rackLocation;
    }

    public Integer getMaxFansSupported() {
        return maxFansSupported;
    }

    public Integer getDesiredSpeedPwm() {
        return desiredSpeedPwm;
    }

    public Integer getDesiredSpeedRpm() {
        return desiredSpeedRpm;
    }

    public String getDescription() {
        return description;
    }

    public Status getStatus() {
        return status;
    }

    public String getPresence() {
        return presence;
    }

    public Integer getVolumetricAirflow() {
        return volumetricAirflow;
    }

    public Integer getNumberOfFansPresent() {
        return numberOfFansPresent;
    }

    public RackLocationDto getRackLocationDto() {
        return rackLocationDto;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private List<TemperatureDto> temperatures;
        private List<FanDto> fans;
        private RackLocationDto rackLocation;
        private Integer maxFansSupported;
        private Integer desiredSpeedPwm;
        private Integer desiredSpeedRpm;
        private String description;
        private Status status;
        private String presence;
        private Integer volumetricAirflow;
        private Integer numberOfFansPresent;
        private RackLocationDto rackLocationDto;

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

        public Builder temperatures(List<TemperatureDto> val) {
            temperatures = val;
            return this;
        }

        public Builder fans(List<FanDto> val) {
            fans = val;
            return this;
        }

        public Builder rackLocation(RackLocationDto val) {
            rackLocation = val;
            return this;
        }

        public Builder maxFansSupported(Integer val) {
            maxFansSupported = val;
            return this;
        }

        public Builder desiredSpeedPwm(Integer val) {
            desiredSpeedPwm = val;
            return this;
        }

        public Builder desiredSpeedRpm(Integer val) {
            desiredSpeedRpm = val;
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

        public Builder presence(String val) {
            presence = val;
            return this;
        }

        public Builder volumetricAirflow(Integer val) {
            volumetricAirflow = val;
            return this;
        }

        public Builder numberOfFansPresent(Integer val) {
            numberOfFansPresent = val;
            return this;
        }

        public Builder rackLocationDto(RackLocationDto val) {
            rackLocationDto = val;
            return this;
        }

        public ThermalZoneDto build() {
            return new ThermalZoneDto(this);
        }
    }
}
