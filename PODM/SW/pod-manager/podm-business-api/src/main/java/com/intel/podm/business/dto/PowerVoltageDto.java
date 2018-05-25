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

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
    "@odata.id", "memberId", "name", "sensorNumber", "status", "readingVolts", "upperThresholdNonCritical", "upperThresholdCritical", "upperThresholdFatal",
    "lowerThresholdNonCritical", "lowerThresholdCritical", "lowerThresholdFatal", "minReadingRange", "maxReadingRange", "physicalContext", "relatedItem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerVoltageDto implements Comparable<PowerVoltageDto> {
    @JsonUnwrapped
    @JsonProperty("@odata.id")
    private ODataId oDataId;
    private String memberId;
    private String name;
    private Integer sensorNumber;
    private Status status;
    private BigDecimal readingVolts;
    private BigDecimal upperThresholdNonCritical;
    private BigDecimal upperThresholdCritical;
    private BigDecimal upperThresholdFatal;
    private BigDecimal lowerThresholdNonCritical;
    private BigDecimal lowerThresholdCritical;
    private BigDecimal lowerThresholdFatal;
    private BigDecimal minReadingRange;
    private BigDecimal maxReadingRange;
    private PhysicalContext physicalContext;
    private List<Context> relatedItem = new ArrayList<>();

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

    public BigDecimal getReadingVolts() {
        return readingVolts;
    }

    public void setReadingVolts(BigDecimal readingVolts) {
        this.readingVolts = readingVolts;
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

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public void setPhysicalContext(PhysicalContext physicalContext) {
        this.physicalContext = physicalContext;
    }

    public List<Context> getRelatedItem() {
        return relatedItem;
    }

    public void setRelatedItem(List<Context> relatedItem) {
        this.relatedItem = relatedItem;
    }

    @Override
    public int compareTo(PowerVoltageDto o) {
        return this.getMemberId().compareTo(o.getMemberId());
    }
}
