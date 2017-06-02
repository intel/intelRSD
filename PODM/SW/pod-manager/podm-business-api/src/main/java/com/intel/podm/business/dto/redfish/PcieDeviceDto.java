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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DeviceType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PcieDeviceDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String assetTag;
    private final String manufacturer;
    private final String model;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final DeviceType deviceType;
    private final String firmwareVersion;
    private final Status status;
    private final Links links;

    private PcieDeviceDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        assetTag = builder.assetTag;
        manufacturer = builder.manufacturer;
        model = builder.model;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        deviceType = builder.deviceType;
        firmwareVersion = builder.firmwareVersion;
        status = builder.status;
        links = new Links(builder.chassis, builder.pcieFunctions);
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

    @Override
    public Links getLinks() {
        return links;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getModel() {
        return model;
    }

    public String getSku() {
        return sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public DeviceType getDeviceType() {
        return deviceType;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public Status getStatus() {
        return status;
    }

    public class Links implements RedfishResource.Links {
        private final Set<Context> chassis;
        private final Set<Context> pcieFunctions;

        Links(Set<Context> chassis, Set<Context> pcieFunctions) {

            this.chassis = chassis;
            this.pcieFunctions = pcieFunctions;
        }

        public Set<Context> getChassis() {
            return chassis;
        }

        public Set<Context> getPcieFunctions() {
            return pcieFunctions;
        }
    }

    public static class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private String assetTag;
        private String manufacturer;
        private String model;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private DeviceType deviceType;
        private String firmwareVersion;
        private Status status;
        private Set<Context> chassis;
        private Set<Context> pcieFunctions;

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

        public Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
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

        public Builder sku(String sku) {
            this.sku = sku;
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

        public Builder deviceType(DeviceType deviceType) {
            this.deviceType = deviceType;
            return this;
        }

        public Builder firmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder chassis(Set<Context> chassis) {
            this.chassis = chassis;
            return this;
        }

        public Builder pcieFunctions(Set<Context> pcieFunctions) {
            this.pcieFunctions = pcieFunctions;
            return this;
        }

        public PcieDeviceDto build() {
            return new PcieDeviceDto(this);
        }
    }
}
