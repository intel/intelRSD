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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.common.types.redfish.RedfishDrive;

public final class RedfishLocationDto implements RedfishDrive.Location {
    private final String info;
    private final String infoFormat;

    private RedfishLocationDto(Builder builder) {
        this.info = builder.info;
        this.infoFormat = builder.infoFormat;
    }

    public String getInfo() {
        return info;
    }

    public String getInfoFormat() {
        return infoFormat;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String info;
        private String infoFormat;

        private Builder() {
        }

        public RedfishLocationDto build() {
            return new RedfishLocationDto(this);
        }

        public Builder info(String info) {
            this.info = info;
            return this;
        }

        public Builder infoFormat(String infoFormat) {
            this.infoFormat = infoFormat;
            return this;
        }
    }
}
