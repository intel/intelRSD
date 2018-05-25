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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.ReferenceableMember;
import com.intel.podm.client.reader.ResourceLinks;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#Storage" + VERSION_PATTERN + "StorageController"
})
@SuppressWarnings("checkstyle:MethodCount")
public class StorageControllerResource extends ExternalServiceResourceImpl implements ReferenceableMember {
    private static final String COMPUTER_SYSTEM_ID_GROUP = "computerSystemId";
    private static final Pattern COMPUTER_SYSTEM_URL_PATTERN = Pattern.compile("(?<" + COMPUTER_SYSTEM_ID_GROUP + ">/redfish/v1/Systems/.+?)/");

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
    private List<IdentifierObject> identifiers = new ArrayList<>();

    public String getMemberId() {
        return memberId;
    }

    public Status getStatus() {
        return status;
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

    public String getAssetTag() {
        return assetTag;
    }

    public BigDecimal getSpeedGbps() {
        return speedGbps;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public List<Protocol> getSupportedControllerProtocols() {
        return supportedControllerProtocols;
    }

    public List<Protocol> getSupportedDeviceProtocols() {
        return supportedDeviceProtocols;
    }

    public List<IdentifierObject> getIdentifiers() {
        return identifiers;
    }

    public ResourceSupplier getComputerSystem() {
        ResourceSupplier computerSystemSupplier = null;
        Matcher matcher = COMPUTER_SYSTEM_URL_PATTERN.matcher(oDataId);
        if (matcher.find()) {
            computerSystemSupplier = toSupplier(new ODataId(matcher.group(COMPUTER_SYSTEM_ID_GROUP)));
        }
        return computerSystemSupplier;
    }

    @Override
    public ResourceLinks getLinks() {
        return new ResourceLinks(this);
    }

    @Override
    public URI getUri() {
        return URI.create(oDataId);
    }

    @Override
    public void setUri(URI uri) {

    }
}
