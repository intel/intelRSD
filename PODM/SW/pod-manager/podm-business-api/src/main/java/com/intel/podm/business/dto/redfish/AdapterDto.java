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
import com.intel.podm.common.types.StorageControllerInterface;

public final class AdapterDto {
    private final String id;
    private final String name;
    private final String description;
    private final StorageControllerInterface storageControllerInterface;
    private final String manufacturer;
    private final String model;
    private final Status status;
    private final String busInfo;
    private final Context containedBy;

    private AdapterDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        storageControllerInterface = builder.storageControllerInterface;
        manufacturer = builder.manufacturer;
        model = builder.model;
        status = builder.status;
        busInfo = builder.busInfo;
        containedBy = builder.containedBy;
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

    public String getDescription() {
        return description;
    }

    public StorageControllerInterface getStorageControllerInterface() {
        return storageControllerInterface;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public Status getStatus() {
        return status;
    }

    public String getBusInfo() {
        return busInfo;
    }

    public Context getContainedBy() {
        return containedBy;
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private StorageControllerInterface storageControllerInterface;
        private String manufacturer;
        private String model;
        private Status status;
        private String busInfo;
        private Context containedBy;

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

        public Builder storageControllerInterface(StorageControllerInterface val) {
            storageControllerInterface = val;
            return this;
        }

        public Builder manufacturer(String val) {
            manufacturer = val;
            return this;
        }

        public Builder model(String val) {
            model = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder busInfo(String val) {
            busInfo = val;
            return this;
        }

        public Builder containedBy(Context val) {
            containedBy = val;
            return this;
        }

        public AdapterDto build() {
            return new AdapterDto(this);
        }
    }
}
