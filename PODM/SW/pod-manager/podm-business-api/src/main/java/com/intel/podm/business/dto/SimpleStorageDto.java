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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({"@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "uefiDevicePath", "status", "devices"})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class SimpleStorageDto extends RedfishDto {
    @JsonProperty("UEFIDevicePath")
    private String uefiDevicePath;
    private Status status;
    private List<SimpleStorageDeviceDto> devices = new ArrayList<>();

    public SimpleStorageDto() {
        super("#SimpleStorage.v1_1_0.SimpleStorage");
    }

    public String getUefiDevicePath() {
        return uefiDevicePath;
    }

    public void setUefiDevicePath(String uefiDevicePath) {
        this.uefiDevicePath = uefiDevicePath;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public List<SimpleStorageDeviceDto> getDevices() {
        return devices;
    }

    public void setDevices(List<SimpleStorageDeviceDto> devices) {
        this.devices = devices;
    }

    @JsonPropertyOrder({"name", "manufacturer", "model", "status", "capacityBytes", "oem"})
    public static final class SimpleStorageDeviceDto {
        private String name;
        private String manufacturer;
        private String model;
        private Status status;
        private BigDecimal capacityBytes;
        @IgnoreAutomaticOem
        private JsonNode oem;

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
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

        public Status getStatus() {
            return status;
        }

        public void setStatus(Status status) {
            this.status = status;
        }

        public BigDecimal getCapacityBytes() {
            return capacityBytes;
        }

        public void setCapacityBytes(BigDecimal capacityBytes) {
            this.capacityBytes = capacityBytes;
        }

        public JsonNode getOem() {
            return oem;
        }

        public void setOem(JsonNode oem) {
            this.oem = oem;
        }
    }
}
