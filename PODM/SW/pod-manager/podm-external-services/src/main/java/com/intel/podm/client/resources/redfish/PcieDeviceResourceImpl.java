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
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.PcieDeviceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DeviceType;
import com.intel.podm.common.types.Status;

import java.util.List;

@OdataTypes({
    "#PCIeDevice" + OdataTypes.VERSION_PATTERN + "PCIeDevice"
})
public class PcieDeviceResourceImpl extends ExternalServiceResourceImpl implements PcieDeviceResource {
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

    @Override
    public String getAssetTag() {
        return assetTag;
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
    public String getSku() {
        return sku;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public String getPartNumber() {
        return partNumber;
    }

    @Override
    public DeviceType getDeviceType() {
        return deviceType;
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
    @LinkName("deviceInChassis")
    public Iterable<ResourceSupplier> getChassis() throws ExternalServiceApiReaderException {
        return toSuppliers(links.chassis);
    }

    @Override
    @LinkName("pcieDeviceFunctions")
    public Iterable<ResourceSupplier> getManagedBy() throws ExternalServiceApiReaderException {
        return toSuppliers(links.pcieFunctions);
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Chassis")
        private List<ODataId> chassis;

        @JsonProperty("PCIeFunctions")
        private List<ODataId> pcieFunctions;
    }
}
