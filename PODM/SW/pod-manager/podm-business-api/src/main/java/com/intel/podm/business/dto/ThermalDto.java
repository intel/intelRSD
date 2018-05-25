/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Set;
import java.util.TreeSet;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "temperatures", "fans", "redundancies", "oem"})
public final class ThermalDto extends RedfishDto {
    private Status status;
    private final Oem oem = new Oem();
    private Set<ThermalTemperatureDto> temperatures = new TreeSet<>();
    private Set<ThermalFanDto> fans = new TreeSet<>();
    @JsonProperty("Redundancy")
    private Set<RedundancyDto> redundancies = new TreeSet<>();

    public ThermalDto() {
        super("#Thermal.v1_1_0.Thermal");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Oem getOem() {
        return oem;
    }

    public Set<ThermalTemperatureDto> getTemperatures() {
        return temperatures;
    }

    public void setTemperatures(Set<ThermalTemperatureDto> temperatures) {
        this.temperatures = temperatures;
    }

    public Set<ThermalFanDto> getFans() {
        return fans;
    }

    public void setFans(Set<ThermalFanDto> fans) {
        this.fans = fans;
    }

    public Set<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public void setRedundancies(Set<RedundancyDto> redundancies) {
        this.redundancies = redundancies;
    }

    @JsonPropertyOrder({
        "@odata.id", "memberId", "name", "sensorNumber", "status", "readingCelsius", "upperThresholdNonCritical", "upperThresholdCritical",
        "upperThresholdFatal", "lowerThresholdNonCritical", "lowerThresholdCritical", "lowerThresholdFatal", "minReadingRangeTemp", "maxReadingRangeTemp",
        "physicalContext", "relatedItem"
    })
    @SuppressWarnings({"checkstyle:MethodCount"})
    public static final class ThermalTemperatureDto implements Comparable<ThermalTemperatureDto> {
        @JsonUnwrapped
        @JsonProperty("@odata.id")
        private ODataId oDataId;
        private String memberId;
        private String name;
        private Integer sensorNumber;
        private Status status;
        private BigDecimal readingCelsius;
        private BigDecimal upperThresholdNonCritical;
        private BigDecimal upperThresholdCritical;
        private BigDecimal upperThresholdFatal;
        private BigDecimal lowerThresholdNonCritical;
        private BigDecimal lowerThresholdCritical;
        private BigDecimal lowerThresholdFatal;
        private BigDecimal minReadingRangeTemp;
        private BigDecimal maxReadingRangeTemp;
        private PhysicalContext physicalContext;
        private Set<Context> relatedItem = new HashSet<>();

        public ODataId getoDataId() {
            return oDataId;
        }

        public void setoDataId(ODataId oDataId) {
            this.oDataId = oDataId;
        }

        public String getMemberId() {
            return memberId;
        }

        public void setMemberId(String memberId) {
            this.memberId = memberId;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public Integer getSensorNumber() {
            return sensorNumber;
        }

        public void setSensorNumber(Integer sensorNumber) {
            this.sensorNumber = sensorNumber;
        }

        public Status getStatus() {
            return status;
        }

        public void setStatus(Status status) {
            this.status = status;
        }

        public BigDecimal getReadingCelsius() {
            return readingCelsius;
        }

        public void setReadingCelsius(BigDecimal readingCelsius) {
            this.readingCelsius = readingCelsius;
        }

        public BigDecimal getUpperThresholdNonCritical() {
            return upperThresholdNonCritical;
        }

        public void setUpperThresholdNonCritical(BigDecimal upperThresholdNonCritical) {
            this.upperThresholdNonCritical = upperThresholdNonCritical;
        }

        public BigDecimal getUpperThresholdCritical() {
            return upperThresholdCritical;
        }

        public void setUpperThresholdCritical(BigDecimal upperThresholdCritical) {
            this.upperThresholdCritical = upperThresholdCritical;
        }

        public BigDecimal getUpperThresholdFatal() {
            return upperThresholdFatal;
        }

        public void setUpperThresholdFatal(BigDecimal upperThresholdFatal) {
            this.upperThresholdFatal = upperThresholdFatal;
        }

        public BigDecimal getLowerThresholdNonCritical() {
            return lowerThresholdNonCritical;
        }

        public void setLowerThresholdNonCritical(BigDecimal lowerThresholdNonCritical) {
            this.lowerThresholdNonCritical = lowerThresholdNonCritical;
        }

        public BigDecimal getLowerThresholdCritical() {
            return lowerThresholdCritical;
        }

        public void setLowerThresholdCritical(BigDecimal lowerThresholdCritical) {
            this.lowerThresholdCritical = lowerThresholdCritical;
        }

        public BigDecimal getLowerThresholdFatal() {
            return lowerThresholdFatal;
        }

        public void setLowerThresholdFatal(BigDecimal lowerThresholdFatal) {
            this.lowerThresholdFatal = lowerThresholdFatal;
        }

        public BigDecimal getMinReadingRangeTemp() {
            return minReadingRangeTemp;
        }

        public void setMinReadingRangeTemp(BigDecimal minReadingRangeTemp) {
            this.minReadingRangeTemp = minReadingRangeTemp;
        }

        public BigDecimal getMaxReadingRangeTemp() {
            return maxReadingRangeTemp;
        }

        public void setMaxReadingRangeTemp(BigDecimal maxReadingRangeTemp) {
            this.maxReadingRangeTemp = maxReadingRangeTemp;
        }

        public PhysicalContext getPhysicalContext() {
            return physicalContext;
        }

        public void setPhysicalContext(PhysicalContext physicalContext) {
            this.physicalContext = physicalContext;
        }

        public Set<Context> getRelatedItem() {
            return relatedItem;
        }

        public void setRelatedItem(Set<Context> relatedItem) {
            this.relatedItem = relatedItem;
        }

        @Override
        public int compareTo(ThermalTemperatureDto other) {
            return this.getMemberId().compareTo(other.getMemberId());
        }
    }

    @JsonPropertyOrder({
        "@odata.id", "@odata.type", "memberId", "name", "physicalContext", "status", "reading", "readingUnits", "upperThresholdNonCritical",
        "upperThresholdCritical", "upperThresholdFatal", "lowerThresholdNonCritical", "lowerThresholdCritical", "lowerThresholdFatal", "minReadingRange",
        "maxReadingRange", "redundancy", "relatedItem"
    })
    @SuppressWarnings({"checkstyle:MethodCount"})
    public static final class ThermalFanDto implements Comparable<ThermalFanDto> {
        @JsonProperty("@odata.type")
        private final String oDataType = "#Thermal.v1_1_0.Fan";
        @JsonUnwrapped
        @JsonProperty("@odata.id")
        private ODataId oDataId;
        private String memberId;
        private String name;
        private PhysicalContext physicalContext;
        private Status status;
        private Integer reading;
        private String readingUnits;
        private Integer upperThresholdNonCritical;
        private Integer upperThresholdCritical;
        private Integer upperThresholdFatal;
        private Integer lowerThresholdNonCritical;
        private Integer lowerThresholdCritical;
        private Integer lowerThresholdFatal;
        private Integer minReadingRange;
        private Integer maxReadingRange;
        private Set<Context> redundancy;
        private Set<Context> relatedItem;

        public ODataId getoDataId() {
            return oDataId;
        }

        public void setoDataId(ODataId oDataId) {
            this.oDataId = oDataId;
        }

        public String getoDataType() {
            return oDataType;
        }

        public String getMemberId() {
            return memberId;
        }

        public void setMemberId(String memberId) {
            this.memberId = memberId;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public PhysicalContext getPhysicalContext() {
            return physicalContext;
        }

        public void setPhysicalContext(PhysicalContext physicalContext) {
            this.physicalContext = physicalContext;
        }

        public Status getStatus() {
            return status;
        }

        public void setStatus(Status status) {
            this.status = status;
        }

        public Integer getReading() {
            return reading;
        }

        public void setReading(Integer reading) {
            this.reading = reading;
        }

        public String getReadingUnits() {
            return readingUnits;
        }

        public void setReadingUnits(String readingUnits) {
            this.readingUnits = readingUnits;
        }

        public Integer getUpperThresholdNonCritical() {
            return upperThresholdNonCritical;
        }

        public void setUpperThresholdNonCritical(Integer upperThresholdNonCritical) {
            this.upperThresholdNonCritical = upperThresholdNonCritical;
        }

        public Integer getUpperThresholdCritical() {
            return upperThresholdCritical;
        }

        public void setUpperThresholdCritical(Integer upperThresholdCritical) {
            this.upperThresholdCritical = upperThresholdCritical;
        }

        public Integer getUpperThresholdFatal() {
            return upperThresholdFatal;
        }

        public void setUpperThresholdFatal(Integer upperThresholdFatal) {
            this.upperThresholdFatal = upperThresholdFatal;
        }

        public Integer getLowerThresholdNonCritical() {
            return lowerThresholdNonCritical;
        }

        public void setLowerThresholdNonCritical(Integer lowerThresholdNonCritical) {
            this.lowerThresholdNonCritical = lowerThresholdNonCritical;
        }

        public Integer getLowerThresholdCritical() {
            return lowerThresholdCritical;
        }

        public void setLowerThresholdCritical(Integer lowerThresholdCritical) {
            this.lowerThresholdCritical = lowerThresholdCritical;
        }

        public Integer getLowerThresholdFatal() {
            return lowerThresholdFatal;
        }

        public void setLowerThresholdFatal(Integer lowerThresholdFatal) {
            this.lowerThresholdFatal = lowerThresholdFatal;
        }

        public Integer getMinReadingRange() {
            return minReadingRange;
        }

        public void setMinReadingRange(Integer minReadingRange) {
            this.minReadingRange = minReadingRange;
        }

        public Integer getMaxReadingRange() {
            return maxReadingRange;
        }

        public void setMaxReadingRange(Integer maxReadingRange) {
            this.maxReadingRange = maxReadingRange;
        }

        public Set<Context> getRedundancy() {
            return redundancy;
        }

        public void setRedundancy(Set<Context> redundancy) {
            this.redundancy = redundancy;
        }

        public Set<Context> getRelatedItem() {
            return relatedItem;
        }

        public void setRelatedItem(Set<Context> relatedItem) {
            this.relatedItem = relatedItem;
        }

        @Override
        public int compareTo(ThermalFanDto other) {
            return this.getMemberId().compareTo(other.getMemberId());
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "desiredSpeedPwm", "volumetricAirflowCfm"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String oDataType = "#Intel.Oem.Thermal";
            private Integer desiredSpeedPwm;
            private Integer volumetricAirflowCfm;

            public String getoDataType() {
                return oDataType;
            }

            public void setDesiredSpeedPwm(Integer desiredSpeedPwm) {
                this.desiredSpeedPwm = desiredSpeedPwm;
            }

            public void setVolumetricAirflowCfm(Integer volumetricAirflowCfm) {
                this.volumetricAirflowCfm = volumetricAirflowCfm;
            }
        }
    }
}
