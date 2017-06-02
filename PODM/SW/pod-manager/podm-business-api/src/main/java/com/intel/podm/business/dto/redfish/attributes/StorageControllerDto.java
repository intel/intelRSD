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

import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.DurableIdentifier;

import java.math.BigDecimal;
import java.util.Collection;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount"})
public final class StorageControllerDto {
    private final String memberId;
    private final Status status;
    private final String manufacturer;
    private final String model;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final String assetTag;
    private final BigDecimal speedGbps;
    private final String firmwareVersion;
    private final Collection<Protocol> supportedControllerProtocols;
    private final Collection<Protocol> supportedDeviceProtocols;
    private final List<DurableIdentifier> identifiers;

    private StorageControllerDto(StorageControllerDto.Builder builder) {
        memberId = builder.memberId;
        assetTag = builder.assetTag;
        manufacturer = builder.manufacturer;
        model = builder.model;
        sku = builder.sku;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        firmwareVersion = builder.firmwareVersion;
        status = builder.status;
        speedGbps = builder.speedGbps;
        supportedControllerProtocols = builder.supportedControllerProtocols;
        supportedDeviceProtocols = builder.supportedDeviceProtocols;
        identifiers = builder.identifiers;
    }

    public static StorageControllerDto.Builder newBuilder() {
        return new StorageControllerDto.Builder();
    }

    public String getMemberId() {
        return memberId;
    }

    public BigDecimal getSpeedGbps() {
        return speedGbps;
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

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public Status getStatus() {
        return status;
    }

    public Collection<Protocol> getSupportedControllerProtocols() {
        return supportedControllerProtocols;
    }

    public Collection<Protocol> getSupportedDeviceProtocols() {
        return supportedDeviceProtocols;
    }

    public List<DurableIdentifier> getIdentifiers() {
        return identifiers;
    }

    public static class Builder {
        private String memberId;
        private Status status;
        private String manufacturer;
        private String model;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private String assetTag;
        private String firmwareVersion;
        private BigDecimal speedGbps;
        private Collection<Protocol> supportedControllerProtocols;
        private Collection<Protocol> supportedDeviceProtocols;
        private List<DurableIdentifier> identifiers;

        public StorageControllerDto.Builder memberId(String memberId) {
            this.memberId = memberId;
            return this;
        }

        public StorageControllerDto.Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
            return this;
        }

        public StorageControllerDto.Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public StorageControllerDto.Builder model(String model) {
            this.model = model;
            return this;
        }

        public StorageControllerDto.Builder sku(String sku) {
            this.sku = sku;
            return this;
        }

        public StorageControllerDto.Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public StorageControllerDto.Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public StorageControllerDto.Builder firmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
            return this;
        }

        public StorageControllerDto.Builder status(Status status) {
            this.status = status;
            return this;
        }

        public StorageControllerDto.Builder supportedControllerProtocols(Collection<Protocol> supportedControllerProtocols) {
            this.supportedControllerProtocols = supportedControllerProtocols;
            return this;
        }

        public StorageControllerDto.Builder supportedDeviceProtocols(Collection<Protocol> supportedDeviceProtocols) {
            this.supportedDeviceProtocols = supportedDeviceProtocols;
            return this;
        }

        public StorageControllerDto.Builder identifiers(List<DurableIdentifier> identifiers) {
            this.identifiers = identifiers;
            return this;
        }

        public StorageControllerDto.Builder speedGbps(BigDecimal speedGbps) {
            this.speedGbps = speedGbps;
            return this;
        }

        public StorageControllerDto build() {
            return new StorageControllerDto(this);
        }
    }
}
