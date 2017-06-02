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
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class EthernetSwitchDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String switchId;
    private final String manufacturer;
    private final String model;
    private final String manufacturingDate;
    private final String serialNumber;
    private final String partNumber;
    private final String firmwareName;
    private final String firmwareVersion;
    private final String role;
    private final Status status;
    private final Links links;

    private EthernetSwitchDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        switchId = builder.switchId;
        manufacturer = builder.manufacturer;
        model = builder.model;
        manufacturingDate = builder.manufacturingDate;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        firmwareName = builder.firmwareName;
        firmwareVersion = builder.firmwareVersion;
        role = builder.role;
        status = builder.status;
        links = new Links(builder.chassis, builder.managedBy);
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

    public String getSwitchId() {
        return switchId;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public String getManufacturingDate() {
        return manufacturingDate;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getFirmwareName() {
        return firmwareName;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public String getRole() {
        return role;
    }

    public Status getStatus() {
        return status;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static final class Links implements RedfishResource.Links {
        private final Context chassis;
        private final Set<Context> managedBy;

        Links(Context chassis, Set<Context> managedBy) {
            this.chassis = chassis;
            this.managedBy = managedBy;
        }

        public Context getChassis() {
            return chassis;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private String switchId;
        private String manufacturer;
        private String model;
        private String manufacturingDate;
        private String serialNumber;
        private String partNumber;
        private String firmwareName;
        private String firmwareVersion;
        private String role;
        private Status status;
        private Context chassis;
        private Set<Context> managedBy;

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

        public Builder switchId(String switchId) {
            this.switchId = switchId;
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

        public Builder manufacturingDate(String manufacturingDate) {
            this.manufacturingDate = manufacturingDate;
            return this;
        }

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public Builder firmwareName(String firmwareName) {
            this.firmwareName = firmwareName;
            return this;
        }

        public Builder firmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
            return this;
        }

        public Builder role(String role) {
            this.role = role;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder chassis(Context chassis) {
            this.chassis = chassis;
            return this;
        }

        public Builder managedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
            return this;
        }

        public EthernetSwitchDto build() {
            return new EthernetSwitchDto(this);
        }
    }
}
