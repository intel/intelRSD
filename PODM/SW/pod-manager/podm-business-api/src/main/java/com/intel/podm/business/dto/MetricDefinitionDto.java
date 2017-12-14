/*
 * Copyright (c) 2017 Intel Corporation
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
import com.intel.podm.common.types.Calculable;
import com.intel.podm.common.types.CalculationAlgorithm;
import com.intel.podm.common.types.DataType;
import com.intel.podm.common.types.Implementation;
import com.intel.podm.common.types.MetricType;
import com.intel.podm.common.types.MetricValueType;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.SensorType;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "sensorType", "implementation", "sensingInterval", "metricType",
    "physicalContext", "units", "minReadingRange", "maxReadingRange", "metricProperties", "precision", "calibration", "isLinear",
    "calculable", "dataType", "accuracy", "timeStampAccuracy", "calculationTimeInterval", "calculationAlgorithm", "discreteValues",
    "calculationParameters", "wildcards", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public final class MetricDefinitionDto extends RedfishDto {
    private SensorType sensorType;
    private Implementation implementation;
    private String sensingInterval;
    private MetricType metricType;
    private PhysicalContext physicalContext;
    private String units;
    private BigDecimal minReadingRange;
    private BigDecimal maxReadingRange;
    private Collection<String> metricProperties = new ArrayList<>();
    private Integer precision;
    private BigDecimal calibration;
    private Boolean isLinear;
    private Calculable calculable;
    private DataType dataType;
    private BigDecimal accuracy;
    private String timeStampAccuracy;
    private String calculationTimeInterval;
    private CalculationAlgorithm calculationAlgorithm;
    private List<String> discreteValues = new ArrayList<>();
    private List<WildcardDto> wildcards = new ArrayList<>();
    private List<CalculationParamsDto> calculationParameters = new ArrayList<>();
    private Oem oem = new Oem();

    public MetricDefinitionDto() {
        super("#MetricDefinition.v1_0_0.MetricDefinition");
    }

    public Calculable getCalculable() {
        return calculable;
    }

    public void setCalculable(Calculable calculable) {
        this.calculable = calculable;
    }

    public DataType getDataType() {
        return dataType;
    }

    public void setDataType(DataType dataType) {
        this.dataType = dataType;
    }

    public BigDecimal getAccuracy() {
        return accuracy;
    }

    public void setAccuracy(BigDecimal accuracy) {
        this.accuracy = accuracy;
    }

    public String getTimeStampAccuracy() {
        return timeStampAccuracy;
    }

    public void setTimeStampAccuracy(String timeStampAccuracy) {
        this.timeStampAccuracy = timeStampAccuracy;
    }

    public String getCalculationTimeInterval() {
        return calculationTimeInterval;
    }

    public void setCalculationTimeInterval(String calculationTimeInterval) {
        this.calculationTimeInterval = calculationTimeInterval;
    }

    public CalculationAlgorithm getCalculationAlgorithm() {
        return calculationAlgorithm;
    }

    public void setCalculationAlgorithm(CalculationAlgorithm calculationAlgorithm) {
        this.calculationAlgorithm = calculationAlgorithm;
    }

    public List<String> getDiscreteValues() {
        return discreteValues;
    }

    public void setDiscreteValues(List<String> discreteValues) {
        this.discreteValues = discreteValues;
    }

    public SensorType getSensorType() {
        return sensorType;
    }

    public void setSensorType(SensorType sensorType) {
        this.sensorType = sensorType;
    }

    public Implementation getImplementation() {
        return implementation;
    }

    public void setImplementation(Implementation implementation) {
        this.implementation = implementation;
    }

    public String getSensingInterval() {
        return sensingInterval;
    }

    public void setSensingInterval(String sensingInterval) {
        this.sensingInterval = sensingInterval;
    }

    public MetricType getMetricType() {
        return metricType;
    }

    public void setMetricType(MetricType metricType) {
        this.metricType = metricType;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public void setPhysicalContext(PhysicalContext physicalContext) {
        this.physicalContext = physicalContext;
    }

    public String getUnits() {
        return units;
    }

    public void setUnits(String units) {
        this.units = units;
    }

    public BigDecimal getMinReadingRange() {
        return minReadingRange;
    }

    public void setMinReadingRange(BigDecimal minReadingRange) {
        this.minReadingRange = minReadingRange;
    }

    public BigDecimal getMaxReadingRange() {
        return maxReadingRange;
    }

    public void setMaxReadingRange(BigDecimal maxReadingRange) {
        this.maxReadingRange = maxReadingRange;
    }

    public Collection<String> getMetricProperties() {
        return metricProperties;
    }

    public void setMetricProperties(Collection<String> metricProperties) {
        this.metricProperties = metricProperties;
    }

    public Integer getPrecision() {
        return precision;
    }

    public void setPrecision(Integer precision) {
        this.precision = precision;
    }

    public BigDecimal getCalibration() {
        return calibration;
    }

    public void setCalibration(BigDecimal calibration) {
        this.calibration = calibration;
    }

    public List<CalculationParamsDto> getCalculationParameters() {
        return calculationParameters;
    }

    public void setCalculationParameters(List<CalculationParamsDto> calculationParameters) {
        this.calculationParameters = calculationParameters;
    }

    public List<WildcardDto> getWildcards() {
        return wildcards;
    }

    public void setWildcards(List<WildcardDto> wildcards) {
        this.wildcards = wildcards;
    }

    public Boolean getIsLinear() {
        return isLinear;
    }

    public void setLinear(Boolean linear) {
        isLinear = linear;
    }

    public Oem getOem() {
        return oem;
    }

    @JsonPropertyOrder({"sourceMetric", "resultMetric"})
    public static final class CalculationParamsDto {

        private String sourceMetric;

        private String resultMetric;

        public String getSourceMetric() {
            return sourceMetric;
        }

        public void setSourceMetric(String sourceMetric) {
            this.sourceMetric = sourceMetric;
        }

        public String getResultMetric() {
            return resultMetric;
        }

        public void setResultMetric(String resultMetric) {
            this.resultMetric = resultMetric;
        }
    }

    @JsonPropertyOrder({"name", "values"})
    public static final class WildcardDto {

        private String name;

        private List<String> values = new ArrayList<>();

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public List<String> getValues() {
            return values;
        }

        public void setValues(List<String> values) {
            this.values = values;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "calculationPrecision", "discreteMetricType"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String odataType = "#Intel.Oem.MetricDefinition";
            private Double calculationPrecision;
            private MetricValueType discreteMetricType;

            public String getOdataType() {
                return odataType;
            }

            public Double getCalculationPrecision() {
                return calculationPrecision;
            }

            public void setCalculationPrecision(Double calculationPrecision) {
                this.calculationPrecision = calculationPrecision;
            }

            public MetricValueType getDiscreteMetricType() {
                return discreteMetricType;
            }

            public void setDiscreteMetricType(MetricValueType discreteMetricType) {
                this.discreteMetricType = discreteMetricType;
            }
        }
    }
}
