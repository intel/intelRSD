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
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

import java.math.BigDecimal;
import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "controllerInterface", "capacityGib", "type", "rpm", "manufacturer",
    "model", "serialNumber", "status", "oem", "links"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PhysicalDriveDto extends RedfishDto {
    @JsonProperty("Interface")
    private StorageControllerInterface controllerInterface;
    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;
    private MediaType type;
    @JsonProperty("RPM")
    private Integer rpm;
    private String manufacturer;
    private String model;
    private String serialNumber;
    private Status status;
    private final Links links = new Links();

    public PhysicalDriveDto() {
        super("#PhysicalDrive.v1_0_0.PhysicalDrive");
    }

    public StorageControllerInterface getControllerInterface() {
        return controllerInterface;
    }

    public void setControllerInterface(StorageControllerInterface controllerInterface) {
        this.controllerInterface = controllerInterface;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public void setCapacityGib(BigDecimal capacityGib) {
        this.capacityGib = capacityGib;
    }

    public MediaType getType() {
        return type;
    }

    public void setType(MediaType type) {
        this.type = type;
    }

    public Integer getRpm() {
        return rpm;
    }

    public void setRpm(Integer rpm) {
        this.rpm = rpm;
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

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
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

    @JsonPropertyOrder({"usedBy", "oem"})
    public final class Links extends RedfishLinksDto {
        private final Set<Context> usedBy = new HashSet<>();

        public Set<Context> getUsedBy() {
            return usedBy;
        }
    }
}
