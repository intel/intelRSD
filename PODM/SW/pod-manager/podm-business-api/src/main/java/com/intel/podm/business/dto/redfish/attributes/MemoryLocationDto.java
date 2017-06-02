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

public final class MemoryLocationDto {
    private final Integer locationSocket;
    private final Integer locationMemoryController;
    private final Integer locationChannel;
    private final Integer locationSlot;

    private MemoryLocationDto(Builder builder) {
        locationSocket = builder.locationSocket;
        locationMemoryController = builder.locationMemoryController;
        locationChannel = builder.locationChannel;
        locationSlot = builder.locationSlot;
    }

    public Integer getLocationSocket() {
        return locationSocket;
    }

    public Integer getLocationMemoryController() {
        return locationMemoryController;
    }

    public Integer getLocationChannel() {
        return locationChannel;
    }

    public Integer getLocationSlot() {
        return locationSlot;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private Integer locationSocket;
        private Integer locationMemoryController;
        private Integer locationChannel;
        private Integer locationSlot;

        public Builder locationSocket(Integer locationSocket) {
            this.locationSocket = locationSocket;
            return this;
        }

        public Builder locationMemoryController(Integer locationMemoryController) {
            this.locationMemoryController = locationMemoryController;
            return this;
        }

        public Builder locationChannel(Integer locationChannel) {
            this.locationChannel = locationChannel;
            return this;
        }

        public Builder locationSlot(Integer locationSlot) {
            this.locationSlot = locationSlot;
            return this;
        }

        public MemoryLocationDto build() {
            return new MemoryLocationDto(this);
        }
    }
}
