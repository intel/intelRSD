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

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerVoltageDto {
    private final Context context;
    private final String memberId;
    private final String name;
    private final Integer sensorNumber;
    private final Status status;
    private final BigDecimal readingVolts;
    private final BigDecimal upperThresholdNonCritical;
    private final BigDecimal upperThresholdCritical;
    private final BigDecimal upperThresholdFatal;
    private final BigDecimal lowerThresholdNonCritical;
    private final BigDecimal lowerThresholdCritical;
    private final BigDecimal lowerThresholdFatal;
    private final BigDecimal minReadingRange;
    private final BigDecimal maxReadingRange;
    private final PhysicalContext physicalContext;
    private final List<Context> relatedItem;

    private PowerVoltageDto(Builder builder) {
        this.context = builder.context;
        this.memberId = builder.memberId;
        this.name = builder.name;
        this.sensorNumber = builder.sensorNumber;
        this.status = builder.status;
        this.readingVolts = builder.readingVolts;
        this.upperThresholdNonCritical = builder.upperThresholdNonCritical;
        this.upperThresholdCritical = builder.upperThresholdCritical;
        this.upperThresholdFatal = builder.upperThresholdFatal;
        this.lowerThresholdNonCritical = builder.lowerThresholdNonCritical;
        this.lowerThresholdCritical = builder.lowerThresholdCritical;
        this.lowerThresholdFatal = builder.lowerThresholdFatal;
        this.minReadingRange = builder.minReadingRange;
        this.maxReadingRange = builder.maxReadingRange;
        this.physicalContext = builder.physicalContext;
        this.relatedItem = builder.relatedItem;
    }

    public Context getContext() {
        return context;
    }

    public String getMemberId() {
        return memberId;
    }

    public String getName() {
        return name;
    }

    public Integer getSensorNumber() {
        return sensorNumber;
    }

    public Status getStatus() {
        return status;
    }

    public BigDecimal getReadingVolts() {
        return readingVolts;
    }

    public BigDecimal getUpperThresholdNonCritical() {
        return upperThresholdNonCritical;
    }

    public BigDecimal getUpperThresholdCritical() {
        return upperThresholdCritical;
    }

    public BigDecimal getUpperThresholdFatal() {
        return upperThresholdFatal;
    }

    public BigDecimal getLowerThresholdNonCritical() {
        return lowerThresholdNonCritical;
    }

    public BigDecimal getLowerThresholdCritical() {
        return lowerThresholdCritical;
    }

    public BigDecimal getLowerThresholdFatal() {
        return lowerThresholdFatal;
    }

    public BigDecimal getMinReadingRange() {
        return minReadingRange;
    }

    public BigDecimal getMaxReadingRange() {
        return maxReadingRange;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public List<Context> getRelatedItem() {
        return relatedItem;
    }

    public static Builder newPowerVoltageDto() {
        return new Builder();
    }


    public static final class Builder {
        private Context context;
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
        private List<Context> relatedItem;

        private Builder() { }

        public PowerVoltageDto build() {
            return new PowerVoltageDto(this);
        }

        public Builder context(Context context) {
            this.context = context;
            return this;
        }

        public Builder memberId(String memberId) {
            this.memberId = memberId;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder sensorNumber(Integer sensorNumber) {
            this.sensorNumber = sensorNumber;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder readingVolts(BigDecimal readingVolts) {
            this.readingVolts = readingVolts;
            return this;
        }

        public Builder upperThresholdNonCritical(BigDecimal upperThresholdNonCritical) {
            this.upperThresholdNonCritical = upperThresholdNonCritical;
            return this;
        }

        public Builder upperThresholdCritical(BigDecimal upperThresholdCritical) {
            this.upperThresholdCritical = upperThresholdCritical;
            return this;
        }

        public Builder upperThresholdFatal(BigDecimal upperThresholdFatal) {
            this.upperThresholdFatal = upperThresholdFatal;
            return this;
        }

        public Builder lowerThresholdNonCritical(BigDecimal lowerThresholdNonCritical) {
            this.lowerThresholdNonCritical = lowerThresholdNonCritical;
            return this;
        }

        public Builder lowerThresholdCritical(BigDecimal lowerThresholdCritical) {
            this.lowerThresholdCritical = lowerThresholdCritical;
            return this;
        }

        public Builder lowerThresholdFatal(BigDecimal lowerThresholdFatal) {
            this.lowerThresholdFatal = lowerThresholdFatal;
            return this;
        }

        public Builder minReadingRange(BigDecimal minReadingRange) {
            this.minReadingRange = minReadingRange;
            return this;
        }

        public Builder maxReadingRange(BigDecimal maxReadingRange) {
            this.maxReadingRange = maxReadingRange;
            return this;
        }

        public Builder physicalContext(PhysicalContext physicalContext) {
            this.physicalContext = physicalContext;
            return this;
        }

        public Builder relatedItem(List<Context> relatedItem) {
            this.relatedItem = relatedItem;
            return this;
        }
    }
}
