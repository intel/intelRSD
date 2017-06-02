/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.common.types.MemoryClassification;

public final class MemoryRegionDto {
    private final String regionId;
    private final MemoryClassification memoryClassification;
    private final Integer offsetMib;
    private final Integer sizeMib;

    private MemoryRegionDto(Builder builder) {
        regionId = builder.regionId;
        memoryClassification = builder.memoryClassification;
        offsetMib = builder.offsetMib;
        sizeMib = builder.sizeMib;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getRegionId() {
        return regionId;
    }

    public MemoryClassification getMemoryClassification() {
        return memoryClassification;
    }

    public Integer getOffsetMib() {
        return offsetMib;
    }

    public Integer getSizeMib() {
        return sizeMib;
    }

    public static final class Builder {
        private String regionId;
        private MemoryClassification memoryClassification;
        private Integer offsetMib;
        private Integer sizeMib;

        private Builder() {
        }

        public Builder regionId(String regionId) {
            this.regionId = regionId;
            return this;
        }

        public Builder memoryClassification(MemoryClassification memoryClassification) {
            this.memoryClassification = memoryClassification;
            return this;
        }

        public Builder offsetMib(Integer offsetMib) {
            this.offsetMib = offsetMib;
            return this;
        }

        public Builder sizeMib(Integer sizeMib) {
            this.sizeMib = sizeMib;
            return this;
        }

        public MemoryRegionDto build() {
            return new MemoryRegionDto(this);
        }
    }
}
