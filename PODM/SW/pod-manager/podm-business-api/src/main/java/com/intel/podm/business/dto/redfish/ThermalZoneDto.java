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

import com.intel.podm.business.dto.redfish.attributes.FanDto;
import com.intel.podm.business.dto.redfish.attributes.RackLocationDto;
import com.intel.podm.business.dto.redfish.attributes.TemperatureDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class ThermalZoneDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final List<TemperatureDto> temperatures;
    private final List<FanDto> fans;
    private final RackLocationDto rackLocation;
    private final Integer maxFansSupported;
    private final Integer desiredSpeedPwm;
    private final Integer desiredSpeedRpm;
    private final Status status;
    private final String presence;
    private final Integer volumetricAirflow;
    private final Integer numberOfFansPresent;
    private final RackLocationDto rackLocationDto;

    private ThermalZoneDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        temperatures = builder.temperatures;
        fans = builder.fans;
        rackLocation = builder.rackLocation;
        maxFansSupported = builder.maxFansSupported;
        desiredSpeedPwm = builder.desiredSpeedPwm;
        desiredSpeedRpm = builder.desiredSpeedRpm;
        status = builder.status;
        presence = builder.presence;
        volumetricAirflow = builder.volumetricAirflow;
        numberOfFansPresent = builder.numberOfFansPresent;
        rackLocationDto = builder.rackLocationDto;
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

    @Override
    public Links getLinks() {
        throw new UnsupportedOperationException("Links are not available in this resource");
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private List<TemperatureDto> temperatures;
        private List<FanDto> fans;
        private RackLocationDto rackLocation;
        private Integer maxFansSupported;
        private Integer desiredSpeedPwm;
        private Integer desiredSpeedRpm;
        private Status status;
        private String presence;
        private Integer volumetricAirflow;
        private Integer numberOfFansPresent;
        private RackLocationDto rackLocationDto;

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

        public Builder temperatures(List<TemperatureDto> temperatures) {
            this.temperatures = temperatures;
            return this;
        }

        public Builder fans(List<FanDto> fans) {
            this.fans = fans;
            return this;
        }

        public Builder rackLocation(RackLocationDto rackLocation) {
            this.rackLocation = rackLocation;
            return this;
        }

        public Builder maxFansSupported(Integer maxFansSupported) {
            this.maxFansSupported = maxFansSupported;
            return this;
        }

        public Builder desiredSpeedPwm(Integer desiredSpeedPwm) {
            this.desiredSpeedPwm = desiredSpeedPwm;
            return this;
        }

        public Builder desiredSpeedRpm(Integer desiredSpeedRpm) {
            this.desiredSpeedRpm = desiredSpeedRpm;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder presence(String presence) {
            this.presence = presence;
            return this;
        }

        public Builder volumetricAirflow(Integer volumetricAirflow) {
            this.volumetricAirflow = volumetricAirflow;
            return this;
        }

        public Builder numberOfFansPresent(Integer numberOfFansPresent) {
            this.numberOfFansPresent = numberOfFansPresent;
            return this;
        }

        public Builder rackLocationDto(RackLocationDto rackLocationDto) {
            this.rackLocationDto = rackLocationDto;
            return this;
        }

        public ThermalZoneDto build() {
            return new ThermalZoneDto(this);
        }
    }
}
