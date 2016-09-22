/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.DeviceResource;
import com.intel.podm.client.api.resources.redfish.LuiDeviceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DriveType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.StorageControllerInterface;

import java.math.BigDecimal;

@OdataTypes({
        "#Device.1.0.0.Device",
        "#Device.v1_0_0.Device"
})
public class DeviceResourceImpl extends ExternalServiceResourceImpl implements DeviceResource, LuiDeviceResource {
    @JsonProperty("Interface")
    private StorageControllerInterface storageControllerInterface;
    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;
    @JsonProperty("Type")
    private DriveType driveType;
    @JsonProperty("RPM")
    private Integer rpm;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("SerialNumber")
    private String serialNumber;
    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("BusInfo")
    private String busInfo;
    @JsonProperty("Oem")
    private Object oem;
    @JsonProperty("Links")
    private Links links;

    @Override
    public StorageControllerInterface getInterface() {
        return storageControllerInterface;
    }

    @Override
    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    @Override
    public DriveType getType() {
        return driveType;
    }

    @Override
    public Integer getRpm() {
        return rpm;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public String getBusInfo() {
        return busInfo;
    }

    @Override
    public ResourceSupplier getAdapter() {
        return toSupplier(links.containedBy);
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static class Links {
        @JsonProperty("ContainedBy")
        private ODataId containedBy;
        @JsonProperty("Oem")
        private Object oem;
    }
}
