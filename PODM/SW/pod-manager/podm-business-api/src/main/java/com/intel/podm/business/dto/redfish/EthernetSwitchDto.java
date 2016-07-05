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
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

import java.util.Collection;

public final class EthernetSwitchDto {
    private final Id id;
    private final String switchId;
    private final String name;
    private final String description;
    private final String manufacturer;
    private final String model;
    private final String manufacturingDate;
    private final String serialNumber;
    private final String partNumber;
    private final String firmwareName;
    private final String firmwareVersion;
    private final String role;
    private final Status status;

    private final Context chassis;
    private final Collection<Context> managedBy;

    private EthernetSwitchDto(Builder builder) {
        id = builder.id;
        switchId = builder.switchId;
        name = builder.name;
        description = builder.description;
        manufacturer = builder.manufacturer;
        model = builder.model;
        manufacturingDate = builder.manufacturingDate;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        firmwareName = builder.firmwareName;
        firmwareVersion = builder.firmwareVersion;
        role = builder.role;
        status = builder.status;
        chassis = builder.chassis;
        managedBy = builder.managedBy;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public String getSwitchId() {
        return switchId;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
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

    public Context getChassis() {
        return chassis;
    }

    public Collection<Context> getManagedBy() {
        return managedBy;
    }

    public static final class Builder {
        private Id id;
        private String switchId;
        private String name;
        private String description;
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
        private Collection<Context> managedBy;

        private Builder() {
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder switchId(String val) {
            switchId = val;
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

        public Builder manufacturer(String val) {
            manufacturer = val;
            return this;
        }

        public Builder model(String val) {
            model = val;
            return this;
        }

        public Builder manufacturingDate(String val) {
            manufacturingDate = val;
            return this;
        }

        public Builder serialNumber(String val) {
            serialNumber = val;
            return this;
        }

        public Builder partNumber(String val) {
            partNumber = val;
            return this;
        }

        public Builder firmwareName(String val) {
            firmwareName = val;
            return this;
        }

        public Builder firmwareVersion(String val) {
            firmwareVersion = val;
            return this;
        }

        public Builder role(String val) {
            role = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder chassis(Context val) {
            chassis = val;
            return this;
        }

        public Builder managedBy(Collection<Context> val) {
            managedBy = val;
            return this;
        }

        public EthernetSwitchDto build() {
            return new EthernetSwitchDto(this);
        }
    }
}
