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
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.resources.redfish.IdentifierObject;
import com.intel.podm.client.api.resources.redfish.StorageResource.StorageControllerResource;
import com.intel.podm.client.resources.redfish.properties.IdentifierObjectImpl;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

@OdataTypes({
    "#Storage" + OdataTypes.VERSION_PATTERN + "StorageController"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class StorageControllerResourceImpl implements StorageControllerResource {
    @JsonProperty("@odata.id")
    private String oDataId;

    @JsonProperty("MemberId")
    private String memberId;

    @JsonProperty("Status")
    private Status status;

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

    @JsonProperty("AssetTag")
    private String assetTag;

    @JsonProperty("SpeedGbps")
    private BigDecimal speedGbps;

    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;

    @JsonProperty("SupportedControllerProtocols")
    private List<Protocol> supportedControllerProtocols = new ArrayList<>();

    @JsonProperty("SupportedDeviceProtocols")
    private List<Protocol> supportedDeviceProtocols = new ArrayList<>();

    @JsonProperty("Identifiers")
    private List<IdentifierObjectImpl> identifiers = new ArrayList<>();

    public String getoDataId() {
        return oDataId;
    }

    @Override
    public String getMemberId() {
        return memberId;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public String getFirmwareVersion() {
        return firmwareVersion;
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
    public String getAssetTag() {
        return assetTag;
    }

    @Override
    public BigDecimal getSpeedGbps() {
        return speedGbps;
    }

    @Override
    public List<Protocol> getSupportedControllerProtocols() {
        return supportedControllerProtocols;
    }

    @Override
    public List<Protocol> getSupportedDeviceProtocols() {
        return supportedDeviceProtocols;
    }

    @Override
    public List<IdentifierObject> getIdentifiers() {
        return (List) identifiers;
    }
}
