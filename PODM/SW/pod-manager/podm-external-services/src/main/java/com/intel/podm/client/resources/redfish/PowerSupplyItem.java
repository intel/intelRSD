/*
 * Copyright (c) 2017 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonIgnore;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.ReferenceableMember;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceLinks;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PowerSupplyType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.client.resources.UnknownOemsHelper.convertJsonNodeToString;

@OdataTypes({"PowerResource\\.PowerSupply",
    "#Power" + VERSION_PATTERN + "PowerSupply"})
@SuppressWarnings({"checkstyle:MethodCount"})
public class PowerSupplyItem implements ExternalServiceResource, ReferenceableMember {
    @JsonIgnore
    private WebClient webClient;

    @JsonProperty("@odata.id")
    private String oDataId;

    @JsonProperty("MemberId")
    private String memberId;

    @JsonProperty("Name")
    private String name;

    @JsonProperty("Status")
    private Status status;

    @IgnoreAutomaticOem
    private String oem;

    @JsonProperty("PowerSupplyType")
    private PowerSupplyType powerSupplyType;

    @JsonProperty("LineInputVoltageType")
    private LineInputVoltageType lineInputVoltageType;

    @JsonProperty("LineInputVoltage")
    private BigDecimal lineInputVoltage;

    @JsonProperty("PowerCapacityWatts")
    private BigDecimal powerCapacityWatts;

    @JsonProperty("LastPowerOutputWatts")
    private BigDecimal lastPowerOutputWatts;

    @JsonProperty("Model")
    private String model;

    @JsonProperty("Manufacturer")
    private String manufacturer;

    @JsonProperty("FirmwareVersion")
    private String firmwareVersion;

    @JsonProperty("SerialNumber")
    private String serialNumber;

    @JsonProperty("PartNumber")
    private String partNumber;

    @JsonProperty("SparePartNumber")
    private String sparePartNumber;

    @JsonProperty("InputRanges")
    private List<InputRangeObject> inputRanges = new ArrayList<>();

    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;

    @JsonProperty("Redundancy")
    private List<ODataId> redundancy = new ArrayList<>();

    @JsonProperty("RelatedItem")
    private List<ODataId> relatedItems = new ArrayList<>();

    @Override
    public String getMemberId() {
        return memberId;
    }

    public String getName() {
        return name;
    }

    public Status getStatus() {
        return status;
    }

    public String getOem() {
        return oem;
    }

    @JsonProperty("Oem")
    public void setOem(JsonNode jsonNode) throws JsonProcessingException {
        oem = convertJsonNodeToString(jsonNode);
    }

    public PowerSupplyType getPowerSupplyType() {
        return powerSupplyType;
    }

    public LineInputVoltageType getLineInputVoltageType() {
        return lineInputVoltageType;
    }

    public BigDecimal getLineInputVoltage() {
        return lineInputVoltage;
    }

    public BigDecimal getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public BigDecimal getLastPowerOutputWatts() {
        return lastPowerOutputWatts;
    }

    public String getModel() {
        return model;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public String getSparePartNumber() {
        return sparePartNumber;
    }

    public List<InputRangeObject> getInputRanges() {
        return inputRanges;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    @LinkName("powerSupplyRedundancy")
    public Iterable<ResourceSupplier> redundancy() throws WebClientRequestException {
        return toSuppliers(webClient, redundancy);
    }

    @LinkName("relatedItems")
    public Iterable<ResourceSupplier> relatedItems() throws WebClientRequestException {
        return toSuppliers(webClient, relatedItems);
    }

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

    @Override
    public void setWebClient(WebClient webClient) {
        this.webClient = webClient;
    }

}
