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

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

public final class PowerDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final List<PowerControlDto> powerControls;
    private final List<PowerVoltageDto> voltages;
    private final List<PowerSupplyDto> powerSupplies;
    private final List<RedundancyDto> redundancies;

    private PowerDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        powerControls = builder.powerControls;
        voltages = builder.voltages;
        powerSupplies = builder.powerSupplies;
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

    public List<PowerControlDto> getPowerControls() {
        return powerControls;
    }

    public List<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    @Override
    public Links getLinks() {
        return null;
    }

    public List<PowerVoltageDto> getVoltages() {
        return voltages;
    }

    public List<PowerSupplyDto> getPowerSupplies() {
        return powerSupplies;
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private List<PowerControlDto> powerControls;
        private List<PowerVoltageDto> voltages;
        private List<PowerSupplyDto> powerSupplies;
        private List<RedundancyDto> redundancies;

        private Builder() {
        }

        public PowerDto build() {
            return new PowerDto(this);
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

        public Builder powerControls(List<PowerControlDto> powerControls) {
            this.powerControls = powerControls;
            return this;
        }

        public Builder voltages(List<PowerVoltageDto> voltages) {
            this.voltages = voltages;
            return this;
        }

        public Builder powerSupplies(List<PowerSupplyDto> powerSupplies) {
            this.powerSupplies = powerSupplies;
            return this;
        }

        public Builder redundancies(List<RedundancyDto> redundancies) {
            this.redundancies = redundancies;
            return this;
        }
    }
}
