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
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.PowerLimitException;
import com.intel.podm.common.types.Status;

import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerControlDto {
    private final String memberId;
    private final PowerLimitDto powerLimitDto;
    private final PowerMetricsDto powerMetricDto;
    private final String name;
    private final Status status;
    private final Integer powerAllocatedWatts;
    private final Integer powerAvailableWatts;
    private final Integer powerCapacityWatts;
    private final Integer powerRequestedWatts;
    private final List<Context> relatedItems;
    private final Integer powerConsumedWatts;
    private final Context context;
    private final UnknownOemDto oem;

    private PowerControlDto(Builder builder) {
        this.memberId = builder.memberId;
        this.powerLimitDto = builder.powerLimitDto;
        this.powerMetricDto = builder.powerMetricDto;
        this.name = builder.name;
        this.status = builder.status;
        this.powerAllocatedWatts = builder.powerAllocatedWatts;
        this.powerAvailableWatts = builder.powerAvailableWatts;
        this.powerCapacityWatts = builder.powerCapacityWatts;
        this.powerRequestedWatts = builder.powerRequestedWatts;
        this.powerConsumedWatts = builder.powerConsumedWatts;
        this.relatedItems = builder.relatedItems;
        this.context = builder.context;
        this.oem = builder.oem;
    }

    public Context getContext() {
        return context;
    }

    public String getMemberId() {
        return memberId;
    }

    public PowerLimitDto getPowerLimitDto() {
        return powerLimitDto;
    }

    public PowerMetricsDto getPowerMetricsDto() {
        return powerMetricDto;
    }

    public String getName() {
        return name;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getPowerAllocatedWatts() {
        return powerAllocatedWatts;
    }

    public Integer getPowerAvailableWatts() {
        return powerAvailableWatts;
    }

    public Integer getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public Integer getPowerRequestedWatts() {
        return powerRequestedWatts;
    }

    public Integer getPowerConsumedWatts() {
        return powerConsumedWatts;
    }

    public List<Context> getRelatedItems() {
        return relatedItems;
    }

    public UnknownOemDto getOem() {
        return oem;
    }

    public static PowerLimitBuilder newPowerLimitDto() {
        return new PowerLimitBuilder();
    }

    public static Builder newPowerControlDto() {
        return new Builder();
    }

    public static PowerMetricsBuilder newPowerMetricsDto() {
        return new PowerMetricsBuilder();
    }

    public static class PowerLimitDto {
        private final Integer correctionInMs;
        private final PowerLimitException limitException;
        private final Integer limitInWatts;

        public PowerLimitDto(PowerLimitBuilder builder) {
            this.correctionInMs = builder.correctionInMs;
            this.limitException = builder.limitException;
            this.limitInWatts = builder.limitInWatts;
        }

        public Integer getCorrectionInMs() {
            return correctionInMs;
        }

        public PowerLimitException getLimitException() {
            return limitException;
        }

        public Integer getLimitInWatts() {
            return limitInWatts;
        }
    }

    public static class PowerMetricsDto {
        private final Integer averageConsumedWatts;
        private final Integer intervalInMin;
        private final Integer maxConsumedWatts;
        private final Integer minConsumedWatts;

        public PowerMetricsDto(PowerMetricsBuilder builder) {
            this.averageConsumedWatts = builder.averageConsumedWatts;
            this.intervalInMin = builder.intervalInMin;
            this.maxConsumedWatts = builder.maxConsumedWatts;
            this.minConsumedWatts = builder.minConsumedWatts;
        }

        public Integer getAverageConsumedWatts() {
            return averageConsumedWatts;
        }

        public Integer getIntervalInMin() {
            return intervalInMin;
        }

        public Integer getMaxConsumedWatts() {
            return maxConsumedWatts;
        }

        public Integer getMinConsumedWatts() {
            return minConsumedWatts;
        }
    }

    public static final class PowerMetricsBuilder {
        private Integer averageConsumedWatts;
        private Integer intervalInMin;
        private Integer maxConsumedWatts;
        private Integer minConsumedWatts;

        private PowerMetricsBuilder() {
        }

        public PowerMetricsDto build() {
            return new PowerMetricsDto(this);
        }

        public PowerMetricsBuilder averageConsumedWatts(Integer averageConsumedWatts) {
            this.averageConsumedWatts = averageConsumedWatts;
            return this;
        }

        public PowerMetricsBuilder intervalInMin(Integer intervalInMin) {
            this.intervalInMin = intervalInMin;
            return this;
        }

        public PowerMetricsBuilder maxConsumedWatts(Integer maxConsumedWatts) {
            this.maxConsumedWatts = maxConsumedWatts;
            return this;
        }

        public PowerMetricsBuilder minConsumedWatts(Integer minConsumedWatts) {
            this.minConsumedWatts = minConsumedWatts;
            return this;
        }
    }

    public static final class PowerLimitBuilder {

        private Integer correctionInMs;
        private PowerLimitException limitException;
        private Integer limitInWatts;

        private PowerLimitBuilder() {
        }

        public PowerLimitBuilder correctionInMs(Integer correctionInMs) {
            this.correctionInMs = correctionInMs;
            return this;
        }

        public PowerLimitDto build() {
            return new PowerLimitDto(this);
        }

        public PowerLimitBuilder limitException(PowerLimitException limitException) {
            this.limitException = limitException;
            return this;
        }

        public PowerLimitBuilder limitInWatts(Integer limitInWatts) {
            this.limitInWatts = limitInWatts;
            return this;
        }
    }

    public static final class Builder {
        private String memberId;
        private PowerLimitDto powerLimitDto;
        private PowerMetricsDto powerMetricDto;
        private String name;
        private Status status;
        private Integer powerAllocatedWatts;
        private Integer powerAvailableWatts;
        private Integer powerCapacityWatts;
        private Integer powerRequestedWatts;
        private List<Context> relatedItems;
        private Integer powerConsumedWatts;
        private Context context;
        private UnknownOemDto oem;

        private Builder() {
        }

        public PowerControlDto build() {
            return new PowerControlDto(this);
        }

        public Builder memberId(String memberId) {
            this.memberId = memberId;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder relatedItems(List<Context> contexts) {
            this.relatedItems = contexts;
            return this;
        }

        public Builder powerAllocatedWatts(Integer powerAllocatedWatts) {
            this.powerAllocatedWatts = powerAllocatedWatts;
            return this;
        }

        public Builder powerAvailableWatts(Integer powerAvailableWatts) {
            this.powerAvailableWatts = powerAvailableWatts;
            return this;
        }

        public Builder powerCapacityWatts(Integer powerCapacityWatts) {
            this.powerCapacityWatts = powerCapacityWatts;
            return this;
        }

        public Builder powerRequestedWatts(Integer powerRequestedWatts) {
            this.powerRequestedWatts = powerRequestedWatts;
            return this;
        }

        public Builder powerLimit(PowerLimitDto powerLimitDto) {
            this.powerLimitDto = powerLimitDto;
            return this;
        }

        public Builder powerMetrics(PowerMetricsDto powerMetricsDto) {
            this.powerMetricDto = powerMetricsDto;
            return this;
        }

        public Builder powerConsumedWatts(Integer powerConsumedWatts) {
            this.powerConsumedWatts = powerConsumedWatts;
            return this;
        }

        public Builder context(Context context) {
            this.context = context;
            return this;
        }

        public Builder oem(UnknownOemDto oem) {
            this.oem = oem;
            return this;
        }
    }
}
