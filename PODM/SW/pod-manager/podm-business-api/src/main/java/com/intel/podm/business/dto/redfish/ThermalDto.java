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

import com.intel.podm.business.dto.redfish.attributes.ThermalFanDto;
import com.intel.podm.business.dto.redfish.attributes.ThermalTemperatureDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

public final class ThermalDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final List<ThermalTemperatureDto> temperatures;
    private final List<ThermalFanDto> fans;
    private final List<RedundancyDto> redundancies;

    private ThermalDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        temperatures = builder.temperatures;
        fans = builder.fans;
        redundancies = builder.redundancies;
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

    @Override
    public Links getLinks() {
        return null;
    }

    public List<ThermalTemperatureDto> getTemperatures() {
        return temperatures;
    }

    public List<ThermalFanDto> getFans() {
        return fans;
    }

    public List<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private List<ThermalTemperatureDto> temperatures;
        private List<ThermalFanDto> fans;
        private List<RedundancyDto> redundancies;

        private Builder() {
        }

        public ThermalDto build() {
            return new ThermalDto(this);
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

        public Builder temperatures(List<ThermalTemperatureDto> temperatures) {
            this.temperatures = temperatures;
            return this;
        }

        public Builder fans(List<ThermalFanDto> fans) {
            this.fans = fans;
            return this;
        }

        public Builder redundancies(List<RedundancyDto> redundancies) {
            this.redundancies = redundancies;
            return this;
        }
    }
}
