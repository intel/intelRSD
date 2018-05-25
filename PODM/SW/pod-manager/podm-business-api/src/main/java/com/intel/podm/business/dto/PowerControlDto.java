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
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.PowerLimitException;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
    "@odata.id", "memberId", "name", "powerConsumedWatts", "powerRequestedWatts", "powerAvailableWatts", "powerCapacityWatts", "powerAllocatedWatts",
    "powerMetrics", "powerLimit", "relatedItems", "status", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerControlDto implements Comparable<PowerControlDto> {
    @JsonUnwrapped
    @JsonProperty("@odata.id")
    private ODataId oDataId;
    private String memberId;
    private String name;
    private BigDecimal powerConsumedWatts;
    private BigDecimal powerRequestedWatts;
    private BigDecimal powerAvailableWatts;
    private BigDecimal powerCapacityWatts;
    private BigDecimal powerAllocatedWatts;
    private PowerMetricsDto powerMetrics = new PowerMetricsDto();
    private PowerLimitDto powerLimit = new PowerLimitDto();
    @JsonProperty("RelatedItem")
    private List<Context> relatedItems = new ArrayList<>();
    private Status status;
    @IgnoreAutomaticOem
    private JsonNode oem;

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

    public BigDecimal getPowerConsumedWatts() {
        return powerConsumedWatts;
    }

    public void setPowerConsumedWatts(BigDecimal powerConsumedWatts) {
        this.powerConsumedWatts = powerConsumedWatts;
    }

    public BigDecimal getPowerRequestedWatts() {
        return powerRequestedWatts;
    }

    public void setPowerRequestedWatts(BigDecimal powerRequestedWatts) {
        this.powerRequestedWatts = powerRequestedWatts;
    }

    public BigDecimal getPowerAvailableWatts() {
        return powerAvailableWatts;
    }

    public void setPowerAvailableWatts(BigDecimal powerAvailableWatts) {
        this.powerAvailableWatts = powerAvailableWatts;
    }

    public BigDecimal getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public void setPowerCapacityWatts(BigDecimal powerCapacityWatts) {
        this.powerCapacityWatts = powerCapacityWatts;
    }

    public BigDecimal getPowerAllocatedWatts() {
        return powerAllocatedWatts;
    }

    public void setPowerAllocatedWatts(BigDecimal powerAllocatedWatts) {
        this.powerAllocatedWatts = powerAllocatedWatts;
    }

    public PowerMetricsDto getPowerMetrics() {
        return powerMetrics;
    }

    public void setPowerMetrics(PowerMetricsDto powerMetrics) {
        this.powerMetrics = powerMetrics;
    }

    public PowerLimitDto getPowerLimit() {
        return powerLimit;
    }

    public void setPowerLimit(PowerLimitDto powerLimit) {
        this.powerLimit = powerLimit;
    }

    public List<Context> getRelatedItems() {
        return relatedItems;
    }

    public void setRelatedItems(List<Context> relatedItems) {
        this.relatedItems = relatedItems;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public JsonNode getOem() {
        return oem;
    }

    public void setOem(JsonNode oem) {
        this.oem = oem;
    }

    @Override
    public int compareTo(PowerControlDto other) {
        return this.getMemberId().compareTo(other.getMemberId());
    }

    @JsonPropertyOrder({"intervalInMin", "minConsumedWatts", "maxConsumedWatts", "averageConsumedWatts"})
    public static final class PowerMetricsDto {
        private Integer intervalInMin;
        private BigDecimal minConsumedWatts;
        private BigDecimal maxConsumedWatts;
        private BigDecimal averageConsumedWatts;

        public Integer getIntervalInMin() {
            return intervalInMin;
        }

        public void setIntervalInMin(Integer intervalInMin) {
            this.intervalInMin = intervalInMin;
        }

        public BigDecimal getMinConsumedWatts() {
            return minConsumedWatts;
        }

        public void setMinConsumedWatts(BigDecimal minConsumedWatts) {
            this.minConsumedWatts = minConsumedWatts;
        }

        public BigDecimal getMaxConsumedWatts() {
            return maxConsumedWatts;
        }

        public void setMaxConsumedWatts(BigDecimal maxConsumedWatts) {
            this.maxConsumedWatts = maxConsumedWatts;
        }

        public BigDecimal getAverageConsumedWatts() {
            return averageConsumedWatts;
        }

        public void setAverageConsumedWatts(BigDecimal averageConsumedWatts) {
            this.averageConsumedWatts = averageConsumedWatts;
        }
    }

    @JsonPropertyOrder({"limitInWatts", "limitException", "correctionInMs"})
    public static final class PowerLimitDto {
        private Integer limitInWatts;
        private PowerLimitException limitException;
        private Integer correctionInMs;

        public Integer getLimitInWatts() {
            return limitInWatts;
        }

        public void setLimitInWatts(Integer limitInWatts) {
            this.limitInWatts = limitInWatts;
        }

        public PowerLimitException getLimitException() {
            return limitException;
        }

        public void setLimitException(PowerLimitException limitException) {
            this.limitException = limitException;
        }

        public Integer getCorrectionInMs() {
            return correctionInMs;
        }

        public void setCorrectionInMs(Integer correctionInMs) {
            this.correctionInMs = correctionInMs;
        }
    }
}
