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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DeviceClass;
import com.intel.podm.common.types.FunctionType;
import com.intel.podm.common.types.Status;

import java.util.LinkedHashSet;
import java.util.Set;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#PCIeFunction" + VERSION_PATTERN + "PCIeFunction"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class PcieDeviceFunctionResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("FunctionId")
    private Integer functionId;
    @JsonProperty("DeviceClass")
    private DeviceClass deviceClass;
    @JsonProperty("FunctionType")
    private FunctionType functionType;
    @JsonProperty("DeviceId")
    private String deviceId;
    @JsonProperty("VendorId")
    private String vendorId;
    @JsonProperty("ClassCode")
    private String classCode;
    @JsonProperty("RevisionId")
    private String revisionId;
    @JsonProperty("SubsystemId")
    private String subsystemId;
    @JsonProperty("SubsystemVendorId")
    private String subsystemVendorId;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("Links")
    private Links links = new Links();

    public Integer getFunctionId() {
        return functionId;
    }

    public DeviceClass getDeviceClass() {
        return deviceClass;
    }

    public FunctionType getFunctionType() {
        return functionType;
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

    @LinkName("pcieDevice")
    public ResourceSupplier getPcieDevice() {
        if (links.pcieDevice == null) {
            return null;
        }
        return toSupplier(links.pcieDevice);
    }

    @LinkName("functionOfDrives")
    public Iterable<ResourceSupplier> getDrives() throws WebClientRequestException {
        if (links.drives == null) {
            return null;
        }
        return toSuppliers(links.drives);
    }

    @LinkName("functionStorageControllers")
    public Iterable<ResourceSupplier> getStorageControllers() throws WebClientRequestException {
        if (links.storageControllers == null) {
            return null;
        }
        return toSuppliers(links.storageControllers);
    }

    @LinkName("functionEthernetInterfaces")
    public Iterable<ResourceSupplier> getEthernetInterfaces() throws WebClientRequestException {
        if (links.ethernetInterfaces == null) {
            return null;
        }
        return toSuppliers(links.ethernetInterfaces);
    }

    @JsonPropertyOrder({"drives", "pcieDevice", "storageControllers", "ethernetInterfaces", "oem"})
    public class Links extends RedfishLinks {
        @JsonProperty("Drives")
        private Set<ODataId> drives = new LinkedHashSet<>();
        @JsonProperty("PCIeDevice")
        private ODataId pcieDevice;
        @JsonProperty("StorageControllers")
        private Set<ODataId> storageControllers = new LinkedHashSet<>();
        @JsonProperty("EthernetInterfaces")
        private Set<ODataId> ethernetInterfaces = new LinkedHashSet<>();
    }
}
