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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
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
import java.util.List;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#MetricDefinition" + VERSION_PATTERN + "MetricDefinition"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class MetricDefinitionResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("SensorType")
    private SensorType sensorType;

    @JsonProperty("Implementation")
    private Implementation implementation;

    @JsonProperty("Calculable")
    private Calculable calculable;

    @JsonProperty("SensingInterval")
    private String sensingInterval;

    @JsonProperty("DataType")
    private DataType dataType;

    @JsonProperty("MetricType")
    private MetricType metricType;

    @JsonProperty("PhysicalContext")
    private PhysicalContext physicalContext;

    @JsonProperty("Units")
    private String units;

    @JsonProperty("IsLinear")
    private Boolean isLinear;

    @JsonProperty("MinReadingRange")
    private BigDecimal minReadingRange;

    @JsonProperty("MaxReadingRange")
    private BigDecimal maxReadingRange;

    @JsonProperty("MetricProperties")
    private Set<String> metricProperties;

    @JsonProperty("Wildcards")
    private List<WildcardObject> wildcards = new ArrayList<>();

    @JsonProperty("DiscreteValues")
    private List<String> discreteValues = new ArrayList<>();

    @JsonProperty("CalculationParameters")
    private List<CalculationParamsTypeObject> calculationParameters = new ArrayList<>();

    @JsonProperty("CalculationAlgorithm")
    private CalculationAlgorithm calculationAlgorithm;

    @JsonProperty("Precision")
    private Integer precision;

    @JsonProperty("Calibration")
    private BigDecimal calibration;

    @JsonProperty("Accuracy")
    private BigDecimal accuracy;

    @JsonProperty("TimeStampAccuracy")
    private String timeStampAccuracy;

    @JsonProperty("CalculationTimeInterval")
    private String calculationTimeInterval;

    @JsonProperty("Links")
    private Links links = new Links();

    @JsonProperty("Oem")
    private Oem oem = new Oem();

    public SensorType getSensorType() {
        return sensorType;
    }

    public Implementation getImplementation() {
        return implementation;
    }

    public Calculable getCalculable() {
        return calculable;
    }

    public String getSensingInterval() {
        return sensingInterval;
    }

    public DataType getDataType() {
        return dataType;
    }

    public MetricType getMetricType() {
        return metricType;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public String getUnits() {
        return units;
    }

    public Boolean getLinear() {
        return isLinear;
    }

    public BigDecimal getMinReadingRange() {
        return minReadingRange;
    }

    public BigDecimal getMaxReadingRange() {
        return maxReadingRange;
    }

    public Set<String> getMetricProperties() {
        return metricProperties;
    }

    public List<WildcardObject> getWildcards() {
        return wildcards;
    }

    public List<String> getDiscreteValues() {
        return discreteValues;
    }

    public List<CalculationParamsTypeObject> getCalculationParameters() {
        return calculationParameters;
    }

    public CalculationAlgorithm getCalculationAlgorithm() {
        return calculationAlgorithm;
    }

    public Integer getPrecision() {
        return precision;
    }

    public BigDecimal getCalibration() {
        return calibration;
    }

    public BigDecimal getAccuracy() {
        return accuracy;
    }

    public String getTimeStampAccuracy() {
        return timeStampAccuracy;
    }

    public String getCalculationTimeInterval() {
        return calculationTimeInterval;
    }

    public Double getCalculationPrecision() {
        return oem.rackScaleOem.calculationPrecision;
    }

    public MetricValueType getDiscreteMetricType() {
        return oem.rackScaleOem.discreteMetricType;
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Metrics")
        private List<ODataId> metrics;

        @JsonProperty("Oem")
        private Links.Oem oem = new Links.Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOem {
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("CalculationPrecision")
            private Double calculationPrecision;

            @JsonProperty("DiscreteMetricType")
            private MetricValueType discreteMetricType;
        }
    }
}
