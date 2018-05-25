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

package com.intel.podm.client.resources.legacy;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.MediaType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Device" + VERSION_PATTERN + "Device"
})
public class DeviceResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Interface")
    private Protocol storageControllerInterface;
    @JsonProperty("CapacityGiB")
    private BigDecimal capacityGib;
    @JsonProperty("Type")
    private MediaType driveType;
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
    @JsonProperty("Links")
    private Links links = new Links();

    public Protocol getInterface() {
        return storageControllerInterface;
    }

    public BigDecimal getCapacityGib() {
        return capacityGib;
    }

    public MediaType getType() {
        return driveType;
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

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public Status getStatus() {
        return status;
    }

    public String getBusInfo() {
        return busInfo;
    }

    public ResourceSupplier getAdapter() {
        return toSupplier(links.containedBy);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("ContainedBy")
        private ODataId containedBy;
    }
}
