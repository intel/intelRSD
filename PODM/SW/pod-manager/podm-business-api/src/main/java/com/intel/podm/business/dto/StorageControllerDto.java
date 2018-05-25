/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Objects;

@JsonPropertyOrder({
    "@odata.id", "@odata.type", "@odata.context", "id", "name", "description", "memberId", "status", "manufacturer", "model", "sku", "serialNumber",
    "partNumber", "assetTag", "speedGbps", "firmwareVersion", "supportedControllerProtocols", "supportedDeviceProtocols", "identifiers", "oem"
})
@JsonIgnoreProperties({"Id", "Name", "Description"})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class StorageControllerDto extends RedfishDto implements Comparable<StorageControllerDto> {
    @JsonProperty("@odata.id")
    private String oDataId;
    private String memberId;
    private Status status;
    private String manufacturer;
    private String model;
    @JsonProperty("SKU")
    private String sku;
    private String serialNumber;
    private String partNumber;
    private String assetTag;
    private BigDecimal speedGbps;
    private String firmwareVersion;
    private Collection<Protocol> supportedControllerProtocols = new ArrayList<>();
    private Collection<Protocol> supportedDeviceProtocols = new ArrayList<>();
    private List<DurableIdentifierDto> identifiers = new ArrayList<>();

    public StorageControllerDto() {
        super("#Storage.v1_0_0.StorageController");
    }

    public String getoDataId() {
        return oDataId;
    }

    public void setoDataId(String oDataId) {
        this.oDataId = oDataId;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public BigDecimal getSpeedGbps() {
        return speedGbps;
    }

    public void setSpeedGbps(BigDecimal speedGbps) {
        this.speedGbps = speedGbps;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public Collection<Protocol> getSupportedControllerProtocols() {
        return supportedControllerProtocols;
    }

    public void setSupportedControllerProtocols(Collection<Protocol> supportedControllerProtocols) {
        this.supportedControllerProtocols = supportedControllerProtocols;
    }

    public Collection<Protocol> getSupportedDeviceProtocols() {
        return supportedDeviceProtocols;
    }

    public void setSupportedDeviceProtocols(Collection<Protocol> supportedDeviceProtocols) {
        this.supportedDeviceProtocols = supportedDeviceProtocols;
    }

    public List<DurableIdentifierDto> getIdentifiers() {
        return identifiers;
    }

    public void setIdentifiers(List<DurableIdentifierDto> identifiers) {
        this.identifiers = identifiers;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof StorageControllerDto)) {
            return false;
        }
        StorageControllerDto that = (StorageControllerDto) o;
        return Objects.equals(getoDataId(), that.getoDataId());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getoDataId());
    }

    @Override
    public int compareTo(StorageControllerDto other) {
        return this.getMemberId().compareTo(other.getMemberId());
    }
}
