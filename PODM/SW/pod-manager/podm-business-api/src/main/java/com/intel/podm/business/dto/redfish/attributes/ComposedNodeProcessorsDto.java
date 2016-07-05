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

import com.intel.podm.common.types.Status;

public final class ComposedNodeProcessorsDto {
    private int count;
    private String model;
    private Status status;

    private ComposedNodeProcessorsDto(Builder builder) {
        this.count = builder.count;
        this.model = builder.model;
        this.status = builder.status;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public int getCount() {
        return count;
    }

    public String getModel() {
        return model;
    }

    public Status getStatus() {
        return status;
    }

    public static final class Builder {
        private int count;
        private String model;
        private Status status;

        private Builder() {
        }

        public Builder count(int count) {
            this.count = count;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public ComposedNodeProcessorsDto build() {
            return new ComposedNodeProcessorsDto(this);
        }
    }
}
