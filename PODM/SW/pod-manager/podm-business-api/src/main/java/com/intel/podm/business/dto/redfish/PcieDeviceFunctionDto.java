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

import com.intel.podm.business.dto.redfish.attributes.StorageControllerDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.DeviceClass;
import com.intel.podm.common.types.FunctionType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount"})
public final class PcieDeviceFunctionDto extends BaseDto implements RedfishResource {
    private String id;
    private String name;
    private String description;
    private Integer functionId;
    private final List<UnknownOemDto> unknownOems;
    private DeviceClass deviceClass;
    private FunctionType functionType;
    private String deviceId;
    private String vendorId;
    private String classCode;
    private String revisionId;
    private String subsystemId;
    private String subsystemVendorId;
    private Status status;
    private final Links links;

    private PcieDeviceFunctionDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        functionId = builder.functionId;
        deviceClass = builder.deviceClass;
        functionType = builder.functionType;
        deviceId = builder.deviceId;
        vendorId = builder.vendorId;
        classCode = builder.classCode;
        revisionId = builder.revisionId;
        subsystemId = builder.subsystemId;
        subsystemVendorId = builder.subsystemVendorId;
        status = builder.status;
        links = new Links(builder.drives, builder.pcieDevice, builder.storageControllers,
            builder.ethernetInterfaces);
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

    public Integer getFunctionId() {
        return functionId;
    }

    public DeviceClass getDeviceClass() {
        return deviceClass;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public String getVendorId() {
        return vendorId;
    }

    public String getClassCode() {
        return classCode;
    }

    public String getRevisionId() {
        return revisionId;
    }

    public String getSubsystemId() {
        return subsystemId;
    }

    public String getSubsystemVendorId() {
        return subsystemVendorId;
    }

    public Status getStatus() {
        return status;
    }

    public FunctionType getFunctionType() {
        return functionType;
    }

    public class Links implements RedfishResource.Links {
        private Set<Context> drives = new LinkedHashSet<>();
        private Context pcieDevice;
        private Set<StorageControllerDto> storageControllers = new LinkedHashSet<>();
        private Set<Context> ethernetInterfaces = new LinkedHashSet<>();

        Links(Set<Context> drives, Context pcieDevice, Set<StorageControllerDto> storageControllers,
              Set<Context> ethernetInterfaces) {
            this.drives = drives;
            this.pcieDevice = pcieDevice;
            this.storageControllers = storageControllers;
            this.ethernetInterfaces = ethernetInterfaces;
        }

        public Set<Context> getDrives() {
            return drives;
        }

        public Context getPcieDevice() {
            return pcieDevice;
        }

        public Set<StorageControllerDto> getStorageControllers() {
            return storageControllers;
        }

        public Set<Context> getEthernetInterfaces() {
            return ethernetInterfaces;
        }
    }

    public static class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Integer functionId;
        private DeviceClass deviceClass;
        private FunctionType functionType;
        private String deviceId;
        private String vendorId;
        private String classCode;
        private String revisionId;
        private String subsystemId;
        private String subsystemVendorId;
        private Status status;
        private Set<Context> drives;
        private Context pcieDevice;
        private Set<StorageControllerDto> storageControllers;
        private Set<Context> ethernetInterfaces;

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

        public Builder functionId(Integer functionId) {
            this.functionId = functionId;
            return this;
        }

        public Builder deviceClass(DeviceClass deviceClass) {
            this.deviceClass = deviceClass;
            return this;
        }

        public Builder functionType(FunctionType functionType) {
            this.functionType = functionType;
            return this;
        }

        public Builder deviceId(String deviceId) {
            this.deviceId = deviceId;
            return this;
        }

        public Builder vendorId(String vendorId) {
            this.vendorId = vendorId;
            return this;
        }

        public Builder classCode(String classCode) {
            this.classCode = classCode;
            return this;
        }

        public Builder revisionId(String revisionId) {
            this.revisionId = revisionId;
            return this;
        }

        public Builder subsystemId(String subsystemId) {
            this.subsystemId = subsystemId;
            return this;
        }

        public Builder subsystemVendorId(String subsystemVendorId) {
            this.subsystemVendorId = subsystemVendorId;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder pcieDevice(Context pcieDevice) {
            this.pcieDevice = pcieDevice;
            return this;
        }

        public Builder drives(Set<Context> drives) {
            this.drives = drives;
            return this;
        }

        public Builder storageControllers(Set<StorageControllerDto> storageControllers) {
            this.storageControllers = storageControllers;
            return this;
        }

        public Builder ethernetInterfaces(Set<Context> ethernetInterfaces) {
            this.ethernetInterfaces = ethernetInterfaces;
            return this;
        }

        public PcieDeviceFunctionDto build() {
            return new PcieDeviceFunctionDto(this);
        }
    }
}
