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
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DeviceType;
import com.intel.podm.common.types.Status;

import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#PCIeDevice" + VERSION_PATTERN + "PCIeDevice"
})
public class PcieDeviceResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("AssetTag")
    private String assetTag;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("SKU")
    private String sku;
    @JsonProperty("SerialNumber")
    private String serialNumber;
    @JsonProperty("PartNumber")
    private String partNumber;
    @JsonProperty("DeviceType")
    private DeviceType deviceType;
    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("AssignableDeviceFunctions")
    private ODataId assignableDeviceFunctions;
    @JsonProperty("Links")
    private Links links = new Links();

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

    public DeviceType getDeviceType() {
        return deviceType;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public Status getStatus() {
        return status;
    }

    @LinkName("deviceInChassis")
    public Iterable<ResourceSupplier> getChassis() throws WebClientRequestException {
        return toSuppliers(links.chassis);
    }

    @LinkName("pcieDeviceFunctions")
    public Iterable<ResourceSupplier> getManagedBy() throws WebClientRequestException {
        return toSuppliers(links.pcieFunctions);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Chassis")
        private List<ODataId> chassis;

        @JsonProperty("PCIeFunctions")
        private List<ODataId> pcieFunctions;
    }
}
