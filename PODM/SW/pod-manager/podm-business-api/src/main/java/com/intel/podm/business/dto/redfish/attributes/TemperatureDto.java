/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.common.types.PhysicalContext;
import com.intel.podm.common.types.Status;

public final class TemperatureDto {
    private final String name;
    private final Status status;
    private final Integer readingCelsius;
    private final PhysicalContext physicalContext;

    private TemperatureDto(Builder builder) {
        name = builder.name;
        status = builder.status;
        readingCelsius = builder.readingCelsius;
        physicalContext = builder.physicalContext;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getName() {
        return name;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getReadingCelsius() {
        return readingCelsius;
    }

    public PhysicalContext getPhysicalContext() {
        return physicalContext;
    }

    public static final class Builder {
        private String name;
        private Status status;
        private Integer readingCelsius;
        private PhysicalContext physicalContext;

        private Builder() {
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder readingCelsius(Integer val) {
            readingCelsius = val;
            return this;
        }

        public Builder physicalContext(PhysicalContext val) {
            physicalContext = val;
            return this;
        }

        public TemperatureDto build() {
            return new TemperatureDto(this);
        }
    }
}
