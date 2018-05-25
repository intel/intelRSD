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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DeviceClass;
import com.intel.podm.common.types.FunctionType;
import com.intel.podm.common.types.Status;

import java.util.Collection;
import java.util.LinkedHashSet;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "functionId", "deviceClass", "functionType", "deviceId", "vendorId",
    "classCode", "revisionId", "subsystemId", "subsystemVendorId", "status", "links", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PcieDeviceFunctionDto extends RedfishDto {
    private final Links links = new Links();
    private Integer functionId;
    @JsonInclude(NON_NULL)
    private DeviceClass deviceClass;
    @JsonInclude(NON_NULL)
    private FunctionType functionType;
    private String deviceId;
    private String vendorId;
    private String classCode;
    private String revisionId;
    private String subsystemId;
    private String subsystemVendorId;
    private Status status;

    public PcieDeviceFunctionDto() {
        super("#PCIeFunction.v1_0_0.PCIeFunction");
    }

    public Integer getFunctionId() {
        return functionId;
    }

    public void setFunctionId(Integer functionId) {
        this.functionId = functionId;
    }

    public DeviceClass getDeviceClass() {
        return deviceClass;
    }

    public void setDeviceClass(DeviceClass deviceClass) {
        this.deviceClass = deviceClass;
    }

    public FunctionType getFunctionType() {
        return functionType;
    }

    public void setFunctionType(FunctionType functionType) {
        this.functionType = functionType;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public String getVendorId() {
        return vendorId;
    }

    public void setVendorId(String vendorId) {
        this.vendorId = vendorId;
    }

    public String getClassCode() {
        return classCode;
    }

    public void setClassCode(String classCode) {
        this.classCode = classCode;
    }

    public String getRevisionId() {
        return revisionId;
    }

    public void setRevisionId(String revisionId) {
        this.revisionId = revisionId;
    }

    public String getSubsystemId() {
        return subsystemId;
    }

    public void setSubsystemId(String subsystemId) {
        this.subsystemId = subsystemId;
    }

    public String getSubsystemVendorId() {
        return subsystemVendorId;
    }

    public void setSubsystemVendorId(String subsystemVendorId) {
        this.subsystemVendorId = subsystemVendorId;
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

    @JsonPropertyOrder({"drives", "pcieDevice", "storageControllers", "ethernetInterfaces", "oem"})
    public class Links extends RedfishLinksDto {
        private final Collection<Context> drives = new LinkedHashSet<>();
        private final Collection<StorageControllerDto> storageControllers = new LinkedHashSet<>();
        private final Collection<Context> ethernetInterfaces = new LinkedHashSet<>();
        @JsonProperty("PCIeDevice")
        private Context pcieDevice;

        public Collection<Context> getDrives() {
            return drives;
        }

        public Context getPcieDevice() {
            return pcieDevice;
        }

        public void setPcieDevice(Context pcieDevice) {
            this.pcieDevice = pcieDevice;
        }

        public Collection<StorageControllerDto> getStorageControllers() {
            return storageControllers;
        }

        public Collection<Context> getEthernetInterfaces() {
            return ethernetInterfaces;
        }
    }
}
