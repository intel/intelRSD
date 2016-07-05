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
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

import java.math.BigDecimal;
import java.util.Collection;

public final class PhysicalDriveDto {
    private final Id id;
    private final String name;
    private final String description;
    private final StorageControllerInterface controllerInterface;
    private final BigDecimal capacityGib;
    private final DriveType type;
    private final Integer rpm;
    private final String manufacturer;
    private final String model;
    private final String serialNumber;
    private final Status status;
    private final Collection<Context> usedBy;
    private final Context context;

    private PhysicalDriveDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        controllerInterface = builder.controllerInterface;
        capacityGib = builder.capacityGib;
        type = builder.type;
        rpm = builder.rpm;
        manufacturer = builder.manufacturer;
        model = builder.model;
        serialNumber = builder.serialNumber;
        status = builder.status;
        usedBy = builder.usedBy;
        context = builder.context;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public StorageControllerInterface getControllerInterface() {
        return controllerInterface;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public DriveType getType() {
        return type;
    }

    public Integer getRpm() {
        return rpm;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public Status getStatus() {
        return status;
    }

    public Collection<Context> getUsedBy() {
        return usedBy;
    }

    public Context getContext() {
        return context;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
        private StorageControllerInterface controllerInterface;
        private BigDecimal capacityGib;
        private DriveType type;
        private Integer rpm;
        private String manufacturer;
        private String model;
        private String serialNumber;
        private Status status;
        private Collection<Context> usedBy;
        private Context context;

        private Builder() {
        }

        public Builder id(Id val) {
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

        public Builder controllerInterface(StorageControllerInterface val) {
            controllerInterface = val;
            return this;
        }

        public Builder capacityGib(BigDecimal val) {
            capacityGib = val;
            return this;
        }

        public Builder type(DriveType val) {
            type = val;
            return this;
        }

        public Builder rpm(Integer val) {
            rpm = val;
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

        public Builder serialNumber(String val) {
            serialNumber = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder usedBy(Collection<Context> val) {
            usedBy = val;
            return this;
        }

        public Builder context(Context val) {
            context = val;
            return this;
        }

        public PhysicalDriveDto build() {
            return new PhysicalDriveDto(this);
        }
    }
}
