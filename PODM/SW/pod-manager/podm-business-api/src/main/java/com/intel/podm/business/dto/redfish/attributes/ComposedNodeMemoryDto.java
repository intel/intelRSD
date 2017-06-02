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

import com.intel.podm.common.types.Status;

import java.math.BigDecimal;

public final class ComposedNodeMemoryDto {
    private BigDecimal totalSystemMemoryGiB;
    private Status status;

    private ComposedNodeMemoryDto(Builder builder) {
        this.totalSystemMemoryGiB = builder.totalSystemMemoryGiB;
        this.status = builder.status;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public BigDecimal getTotalSystemMemoryGiB() {
        return totalSystemMemoryGiB;
    }

    public Status getStatus() {
        return status;
    }

    public static final class Builder {
        private BigDecimal totalSystemMemoryGiB;
        private Status status;

        private Builder() {
        }

        public Builder totalSystemMemoryGiB(BigDecimal totalSystemMemoryGiB) {
            this.totalSystemMemoryGiB = totalSystemMemoryGiB;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public ComposedNodeMemoryDto build() {
            return new ComposedNodeMemoryDto(this);
        }
    }
}
