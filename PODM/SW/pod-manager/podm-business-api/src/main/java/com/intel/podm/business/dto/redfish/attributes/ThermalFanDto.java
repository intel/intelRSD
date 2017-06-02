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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class ThermalFanDto {

    private final Context context;
    private final String memberId;
    private final String name;
    private final PhysicalContext physicalContext;
    private final Status status;
    private final Integer reading;
    private final String readingUnits;
    private final Integer upperThresholdNonCritical;
    private final Integer upperThresholdCritical;
    private final Integer upperThresholdFatal;
    private final Integer lowerThresholdNonCritical;
    private final Integer lowerThresholdCritical;
    private final Integer lowerThresholdFatal;
    private final Integer minReadingRange;
    private final Integer maxReadingRange;
    private final Set<Context> redundancy;
    private final Set<Context> relatedItem;

    @SuppressWarnings({"checkstyle:ExecutableStatementCount"})
    private ThermalFanDto(Builder builder) {
        this.memberId = builder.memberId;
        this.name = builder.name;
        this.physicalContext = builder.physicalContext;
        this.status = builder.status;
        this.reading = builder.reading;
        this.readingUnits = builder.readingUnits;
        this.upperThresholdNonCritical = builder.upperThresholdNonCritical;
        this.upperThresholdCritical = builder.upperThresholdCritical;
        this.upperThresholdFatal = builder.upperThresholdFatal;
        this.lowerThresholdNonCritical = builder.lowerThresholdNonCritical;
        this.lowerThresholdCritical = builder.lowerThresholdCritical;
        this.lowerThresholdFatal = builder.lowerThresholdFatal;
        this.minReadingRange = builder.minReadingRange;
        this.maxReadingRange = builder.maxReadingRange;
        this.redundancy = builder.redundancy;
        this.relatedItem = builder.relatedItem;
        this.context = builder.context;
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

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getReading() {
        return reading;
    }

    public String getReadingUnits() {
        return readingUnits;
    }

    public Integer getUpperThresholdNonCritical() {
        return upperThresholdNonCritical;
    }

    public Integer getUpperThresholdCritical() {
        return upperThresholdCritical;
    }

    public Integer getUpperThresholdFatal() {
        return upperThresholdFatal;
    }

    public Integer getLowerThresholdNonCritical() {
        return lowerThresholdNonCritical;
    }

    public Integer getLowerThresholdCritical() {
        return lowerThresholdCritical;
    }

    public Integer getLowerThresholdFatal() {
        return lowerThresholdFatal;
    }

    public Integer getMinReadingRange() {
        return minReadingRange;
    }

    public Integer getMaxReadingRange() {
        return maxReadingRange;
    }

    public Set<Context> getRedundancy() {
        return redundancy;
    }

    public Set<Context> getRelatedItem() {
        return relatedItem;
    }

    public static Builder newThermalFanDto() {
        return new Builder();
    }

    public static final class Builder {
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
        private Context context;

        private Builder() {
        }

        public ThermalFanDto build() {
            return new ThermalFanDto(this);
        }

        public Builder context(Context fanContext) {
            this.context = fanContext;
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

        public Builder physicalContext(PhysicalContext physicalContext) {
            this.physicalContext = physicalContext;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder reading(Integer reading) {
            this.reading = reading;
            return this;
        }

        public Builder readingUnits(String readingUnits) {
            this.readingUnits = readingUnits;
            return this;
        }

        public Builder upperThresholdNonCritical(Integer upperThresholdNonCritical) {
            this.upperThresholdNonCritical = upperThresholdNonCritical;
            return this;
        }

        public Builder upperThresholdCritical(Integer upperThresholdCritical) {
            this.upperThresholdCritical = upperThresholdCritical;
            return this;
        }

        public Builder upperThresholdFatal(Integer upperThresholdFatal) {
            this.upperThresholdFatal = upperThresholdFatal;
            return this;
        }

        public Builder lowerThresholdNonCritical(Integer lowerThresholdNonCritical) {
            this.lowerThresholdNonCritical = lowerThresholdNonCritical;
            return this;
        }

        public Builder lowerThresholdCritical(Integer lowerThresholdCritical) {
            this.lowerThresholdCritical = lowerThresholdCritical;
            return this;
        }

        public Builder lowerThresholdFatal(Integer lowerThresholdFatal) {
            this.lowerThresholdFatal = lowerThresholdFatal;
            return this;
        }

        public Builder minReadingRange(Integer minReadingRange) {
            this.minReadingRange = minReadingRange;
            return this;
        }

        public Builder maxReadingRange(Integer maxReadingRange) {
            this.maxReadingRange = maxReadingRange;
            return this;
        }

        public Builder redundancy(Set<Context> redundancy) {
            this.redundancy = redundancy;
            return this;
        }

        public Builder relatedItem(Set<Context> relatedItem) {
            this.relatedItem = relatedItem;
            return this;
        }
    }
}
