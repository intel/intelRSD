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

import com.intel.podm.common.types.rmm.RackUnitType;

public final class RackLocationDto {
    private final Integer uLocation;
    private final Integer uHeight;
    private final Integer xLocation;
    private final RackUnitType rackUnitType;

    private RackLocationDto(Builder builder) {
        uLocation = builder.uLocation;
        uHeight = builder.uHeight;
        xLocation = builder.xLocation;
        rackUnitType = builder.rackUnitType;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Integer getuLocation() {
        return uLocation;
    }

    public Integer getuHeight() {
        return uHeight;
    }

    public Integer getxLocation() {
        return xLocation;
    }

    public RackUnitType getRackUnitType() {
        return rackUnitType;
    }

    public static final class Builder {
        private Integer uLocation;
        private Integer uHeight;
        private Integer xLocation;
        private RackUnitType rackUnitType;

        private Builder() {
        }

        public Builder uLocation(Integer uLocation) {
            this.uLocation = uLocation;
            return this;
        }

        public Builder uHeight(Integer uHeight) {
            this.uHeight = uHeight;
            return this;
        }

        public Builder xLocation(Integer xLocation) {
            this.xLocation = xLocation;
            return this;
        }

        public Builder rackUnitType(RackUnitType rackUnitType) {
            this.rackUnitType = rackUnitType;
            return this;
        }

        public RackLocationDto build() {
            return new RackLocationDto(this);
        }
    }
}
