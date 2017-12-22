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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DeviceType;
import com.intel.podm.common.types.Status;

import java.util.Collection;
import java.util.HashSet;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "assetTag", "manufacturer", "model", "sku", "serialNumber", "partNumber",
    "deviceType", "firmwareVersion", "status", "links", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PcieDeviceDto extends RedfishDto {
    private String assetTag;
    private String manufacturer;
    private String model;
    @JsonProperty("SKU")
    private String sku;
    private String serialNumber;
    private String partNumber;
    private DeviceType deviceType;
    private String firmwareVersion;
    private Status status;
    private Links links = new Links();

    public PcieDeviceDto() {
        super("#PCIeDevice.v1_0_0.PCIeDevice");
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
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

    public DeviceType getDeviceType() {
        return deviceType;
    }

    public void setDeviceType(DeviceType deviceType) {
        this.deviceType = deviceType;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    @JsonPropertyOrder({"chassis", "pcieFunctions", "oem"})
    public class Links extends RedfishLinksDto {
        @JsonProperty("Chassis")
        private final Collection<Context> chassis = new HashSet<>();

        @JsonProperty("PCIeFunctions")
        private final Collection<Context> pcieFunctions = new HashSet<>();

        public Collection<Context> getChassis() {
            return chassis;
        }

        public Collection<Context> getPcieFunctions() {
            return pcieFunctions;
        }
    }
}
