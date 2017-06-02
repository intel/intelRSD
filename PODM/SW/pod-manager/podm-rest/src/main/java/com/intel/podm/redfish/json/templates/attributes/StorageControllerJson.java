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

package com.intel.podm.redfish.json.templates.attributes;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.Collection;

@JsonPropertyOrder({
        "@odata.id", "@odata.type", "memberId", "status",
        "manufacturer", "model", "sku", "serialNumber", "partNumber", "assetTag", "speedGbps",
        "firmwareVersion", "supportedControllerProtocols", "supportedDeviceProtocols", "identifiers"
})

@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class StorageControllerJson {
    @JsonUnwrapped
    @JsonProperty("@odata.id")
    public String oDataId;
    @JsonProperty("@odata.type")
    public String oDataType;
    public String memberId;
    public Status status;
    public String manufacturer;
    public String model;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    public String assetTag;
    public BigDecimal speedGbps;
    public String firmwareVersion;
    public Collection<Protocol> supportedControllerProtocols;
    public Collection<Protocol> supportedDeviceProtocols;
    public Collection<IdentifierJson> identifiers;

    public StorageControllerJson() {
        this.oDataType = "#Storage.v1_0_0.StorageController";
    }
}
