/*
 * Copyright (c) 2015 Intel Corporation
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
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.List;

public final class SimpleStorageDto {
    private final String id;
    private final String name;
    private final String description;
    private final String uefiDevicePath;
    private final Status status;
    private final List<SimpleStorageDeviceDto> devices;

    private final Context context;

    private SimpleStorageDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        uefiDevicePath = builder.uefiDevicePath;
        status = builder.status;
        devices = builder.devices;
        context = builder.context;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Context getContext() {
        return context;
    }

    public String getDescription() {
        return description;
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

    public static final class SimpleStorageDeviceDto {
        private final String name;
        private final String manufacturer;
        private final String model;
        private final Status status;
        private final BigDecimal capacityBytes;

        public SimpleStorageDeviceDto(String name, String manufacturer, String model, Status status, BigDecimal capacityBytes) {
            this.name = name;
            this.manufacturer = manufacturer;
            this.model = model;
            this.status = status;
            this.capacityBytes = capacityBytes;
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
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private String uefiDevicePath;
        private Status status;
        private List<SimpleStorageDeviceDto> devices;
        private Context context;

        private Builder() {
        }

        public Builder id(String val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder uefiDevicePath(String val) {
            uefiDevicePath = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder devices(List<SimpleStorageDeviceDto> val) {
            devices = val;
            return this;
        }

        public Builder context(Context val) {
            context = val;
            return this;
        }

        public SimpleStorageDto build() {
            return new SimpleStorageDto(this);
        }
    }
}
