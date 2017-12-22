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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.Set;
import java.util.TreeSet;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name",
    "description", "powerControl", "voltages", "powerSupplies", "redundancy", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerDto extends RedfishDto {
    @JsonProperty("PowerControl")
    private Set<PowerControlDto> powerControls = new TreeSet<>();
    private Set<PowerVoltageDto> voltages = new TreeSet<>();
    private Set<PowerSupplyDto> powerSupplies = new TreeSet<>();
    @JsonProperty("Redundancy")
    private Set<RedundancyDto> redundancies = new TreeSet<>();
    private Oem oem = new Oem();

    public PowerDto() {
        super("#Power.v1_1_0.Power");
    }

    public Set<PowerControlDto> getPowerControls() {
        return powerControls;
    }

    public void setPowerControls(Set<PowerControlDto> powerControls) {
        this.powerControls = powerControls;
    }

    public Set<PowerVoltageDto> getVoltages() {
        return voltages;
    }

    public void setVoltages(Set<PowerVoltageDto> voltages) {
        this.voltages = voltages;
    }

    public Set<PowerSupplyDto> getPowerSupplies() {
        return powerSupplies;
    }

    public void setPowerSupplies(Set<PowerSupplyDto> powerSupplies) {
        this.powerSupplies = powerSupplies;
    }

    public Set<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public void setRedundancies(Set<RedundancyDto> redundancies) {
        this.redundancies = redundancies;
    }

    public Oem getOem() {
        return oem;
    }

    public void setOem(Oem oem) {
        this.oem = oem;
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"oDataType", "inputAcPowerWatts"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Power";
            @JsonProperty("InputACPowerWatts")
            private BigDecimal inputAcPowerWatts;

            public String getoDataType() {
                return oDataType;
            }

            public BigDecimal getInputAcPowerWatts() {
                return inputAcPowerWatts;
            }

            public void setInputAcPowerWatts(BigDecimal inputAcPowerWatts) {
                this.inputAcPowerWatts = inputAcPowerWatts;
            }
        }
    }
}
