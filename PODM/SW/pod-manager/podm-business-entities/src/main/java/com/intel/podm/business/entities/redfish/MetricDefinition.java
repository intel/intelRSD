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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.CalculationParamsType;
import com.intel.podm.common.types.Calculable;
import com.intel.podm.common.types.CalculationAlgorithm;
import com.intel.podm.common.types.DataType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Implementation;
import com.intel.podm.common.types.MetricType;
import com.intel.podm.common.types.MetricValueType;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.SensorType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = MetricDefinition.GET_ALL_METRIC_DEFINITIONS_IDS,
        query = "SELECT metricDefinition.entityId FROM MetricDefinition metricDefinition")
})
@Table(name = "metric_definition", indexes = @Index(name = "idx_metric_definition_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class MetricDefinition extends DiscoverableEntity {
    public static final String GET_ALL_METRIC_DEFINITIONS_IDS = "GET_ALL_METRIC_DEFINITIONS_IDS";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "sensor_type")
    @Enumerated(STRING)
    private SensorType sensorType;

    @Column(name = "calculable")
    @Enumerated(STRING)
    private Calculable calculable;

    @Column(name = "data_type")
    @Enumerated(STRING)
    private DataType dataType;

    @Column(name = "implementation")
    @Enumerated(STRING)
    private Implementation implementation;

    @Column(name = "sensing_interval")
    private String sensingInterval;

    @Column(name = "is_linear")
    private Boolean isLinear;

    @Column(name = "metric_type")
    @Enumerated(STRING)
    private MetricType metricType;

    @Column(name = "physical_context")
    @Enumerated(STRING)
    private PhysicalContext physicalContext;

    @Column(name = "calculation_algorithm")
    @Enumerated(STRING)
    private CalculationAlgorithm calculationAlgorithm;

    @Column(name = "units")
    private String units;

    @Column(name = "min_reading_range")
    private BigDecimal minReadingRange;

    @Column(name = "max_reading_range")
    private BigDecimal maxReadingRange;

    @Column(name = "precision")
    private Integer precision;

    @Column(name = "calibration")
    private BigDecimal calibration;

    @Column(name = "accuracy")
    private BigDecimal accuracy;

    @Column(name = "time_stamp_accuracy")
    private String timeStampAccuracy;

    @Column(name = "calculation_time_interval")
    private String calculationTimeInterval;

    @Column(name = "calculation_precision")
    private Double calculationPrecision;

    @Column(name = "discrete_metric_type")
    @Enumerated(STRING)
    private MetricValueType discreteMetricType;

    @ElementCollection
    @CollectionTable(name = "metric_definition_metric_properties", joinColumns = @JoinColumn(name = "metric_definition_id"))
    @Column(name = "metric_properties")
    @OrderColumn(name = "metric_properties_order")
    private List<String> metricProperties = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "metric_definition_discrete_values", joinColumns = @JoinColumn(name = "metric_definition_id"))
    @Column(name = "discrete_values")
    @OrderColumn(name = "discrete_values_order")
    private List<String> discreteValues = new ArrayList<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "metric_item_id")
    private MetricItem metricItem;

    @IgnoreUnlinkingRelationship
    @OneToMany(fetch = LAZY, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "metric_definition_wildcard_id")
    private List<MetricDefinitionWildcard> wildcards = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "metric_definition_calculation_parameters", joinColumns = @JoinColumn(name = "metric_definition_id"))
    @OrderColumn(name = "calculation_parameters_values_order")
    private List<CalculationParamsType> calculationParameters = new ArrayList<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
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

    public void setUnits(String readingUnits) {
        this.units = readingUnits;
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

    public List<String> getMetricProperties() {
        return metricProperties;
    }

    public void addMetricProperties(String metricProperties) {
        this.metricProperties.add(metricProperties);
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

    public Boolean getLinear() {
        return isLinear;
    }

    public void setLinear(Boolean linear) {
        isLinear = linear;
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

    public List<String> getDiscreteValues() {
        return discreteValues;
    }

    public void addDiscreteValues(String discreteValue) {
        this.discreteValues.add(discreteValue);
    }

    public List<MetricDefinitionWildcard> getWildcards() {
        return wildcards;
    }

    public void addWildcards(MetricDefinitionWildcard wildcard) {
        requiresNonNull(wildcard, "wildcard");
        this.wildcards.add(wildcard);
    }

    public List<CalculationParamsType> getCalculationParameters() {
        return calculationParameters;
    }

    public void addCalculationParameters(CalculationParamsType calculationParameter) {
        this.calculationParameters.add(calculationParameter);
    }

    public MetricItem getMetricItem() {
        return metricItem;
    }

    public void setMetricItem(MetricItem metricItem) {
        if (!Objects.equals(this.metricItem, metricItem)) {
            unlinkMetricItem(this.metricItem);
            this.metricItem = metricItem;
            if (metricItem != null && !this.equals(metricItem.getMetricDefinition())) {
                metricItem.setMetricDefinition(this);
            }
        }
    }

    public void unlinkMetricItem(MetricItem metricItem) {
        if (Objects.equals(this.metricItem, metricItem)) {
            this.metricItem = null;
            if (metricItem != null) {
                metricItem.unlinkMetricDefinition(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkMetricItem(metricItem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
