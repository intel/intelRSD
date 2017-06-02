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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.math.BigDecimal;
import java.util.List;

public final class SimpleStorageDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String uefiDevicePath;
    private final Status status;
    private final List<SimpleStorageDeviceDto> devices;

    private SimpleStorageDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        uefiDevicePath = builder.uefiDevicePath;
        status = builder.status;
        devices = builder.devices;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    public String getUefiDevicePath() {
        return uefiDevicePath;
    }

    public Status getStatus() {
        return status;
    }

    public List<SimpleStorageDeviceDto> getDevices() {
        return devices;
    }

    @Override
    public Links getLinks() {
        throw new UnsupportedOperationException("Links are not available in this resource");
    }

    public static final class SimpleStorageDeviceDto {
        private final String name;
        private final String manufacturer;
        private final String model;
        private final Status status;
        private final BigDecimal capacityBytes;
        private final UnknownOemDto oem;

        private SimpleStorageDeviceDto(Builder builder) {
            this.name = builder.name;
            this.manufacturer = builder.manufacturer;
            this.model = builder.model;
            this.status = builder.status;
            this.capacityBytes = builder.capacityBytes;
            this.oem = builder.oem;
        }

        public static Builder newSimpleStorageDeviceDto() {
            return new Builder();
        }

        public Status getStatus() {
            return status;
        }

        public String getModel() {
            return model;
        }

        public String getManufacturer() {
            return manufacturer;
        }

        public String getName() {
            return name;
        }

        public BigDecimal getCapacityBytes() {
            return capacityBytes;
        }

        public UnknownOemDto getOem() {
            return oem;
        }

        public static final class Builder {
            private String name;
            private String manufacturer;
            private String model;
            private Status status;
            private BigDecimal capacityBytes;
            private UnknownOemDto oem;

            private Builder() {
            }

            public SimpleStorageDeviceDto build() {
                return new SimpleStorageDeviceDto(this);
            }

            public Builder name(String name) {
                this.name = name;
                return this;
            }

            public Builder manufacturer(String manufacturer) {
                this.manufacturer = manufacturer;
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

            public Builder capacityBytes(BigDecimal capacityBytes) {
                this.capacityBytes = capacityBytes;
                return this;
            }

            public Builder oem(UnknownOemDto oem) {
                this.oem = oem;
                return this;
            }
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private String uefiDevicePath;
        private Status status;
        private List<SimpleStorageDeviceDto> devices;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder uefiDevicePath(String status) {
            this.uefiDevicePath = status;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder devices(List<SimpleStorageDeviceDto> status) {
            this.devices = status;
            return this;
        }

        public SimpleStorageDto build() {
            return new SimpleStorageDto(this);
        }
    }
}
