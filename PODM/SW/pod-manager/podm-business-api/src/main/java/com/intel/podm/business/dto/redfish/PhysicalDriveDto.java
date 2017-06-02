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
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PhysicalDriveDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final StorageControllerInterface controllerInterface;
    private final Float capacityGib;
    private final MediaType type;
    private final Integer rpm;
    private final String manufacturer;
    private final String model;
    private final String serialNumber;
    private final Status status;
    private final Links links;

    private PhysicalDriveDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        controllerInterface = builder.controllerInterface;
        capacityGib = builder.capacityGib;
        type = builder.type;
        rpm = builder.rpm;
        manufacturer = builder.manufacturer;
        model = builder.model;
        serialNumber = builder.serialNumber;
        status = builder.status;
        links = new Links(builder.usedBy);
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

    public StorageControllerInterface getControllerInterface() {
        return controllerInterface;
    }

    public Float getCapacityGib() {
        return capacityGib;
    }

    public MediaType getType() {
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

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> usedBy;

        public Links(Set<Context> usedBy) {
            this.usedBy = usedBy;
        }

        public Set<Context> getUsedBy() {
            return usedBy;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private StorageControllerInterface controllerInterface;
        private Float capacityGib;
        private MediaType type;
        private Integer rpm;
        private String manufacturer;
        private String model;
        private String serialNumber;
        private Status status;
        private Set<Context> usedBy;

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

        public Builder controllerInterface(StorageControllerInterface controllerInterface) {
            this.controllerInterface = controllerInterface;
            return this;
        }

        public Builder capacityGib(Float capacityGib) {
            this.capacityGib = capacityGib;
            return this;
        }

        public Builder type(MediaType type) {
            this.type = type;
            return this;
        }

        public Builder rpm(Integer rpm) {
            this.rpm = rpm;
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

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder usedBy(Set<Context> usedBy) {
            this.usedBy = usedBy;
            return this;
        }

        public PhysicalDriveDto build() {
            return new PhysicalDriveDto(this);
        }
    }
}
