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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PowerSupplyType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
    "@odata.id", "@odata.type", "memberId", "name", "status", "powerSupplyType", "lineInputVoltageType", "lineInputVoltage", "powerCapacityWatts",
    "lastPowerOutputWatts", "model", "manufacturer", "firmwareVersion", "serialNumber", "partNumber", "sparePartNumber", "inputRanges", "redundancy",
    "relatedItem", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class PowerSupplyDto implements Comparable<PowerSupplyDto> {
    @JsonProperty("@odata.type")
    private final String oDataType = "#Power.v1_1_0.PowerSupply";
    @JsonUnwrapped
    @JsonProperty("@odata.id")
    private ODataId oDataId;
    private String memberId;
    private String name;
    private Status status;
    private PowerSupplyType powerSupplyType;
    private LineInputVoltageType lineInputVoltageType;
    private BigDecimal lineInputVoltage;
    private BigDecimal powerCapacityWatts;
    private BigDecimal lastPowerOutputWatts;
    private String model;
    private String manufacturer;
    private String firmwareVersion;
    private String serialNumber;
    private String partNumber;
    private String sparePartNumber;
    private List<InputRangeDto> inputRanges = new ArrayList<>();
    private List<Context> redundancy = new ArrayList<>();
    private List<Context> relatedItem = new ArrayList<>();
    @IgnoreAutomaticOem
    private JsonNode oem;

    public ODataId getoDataId() {
        return oDataId;
    }

    public void setoDataId(ODataId oDataId) {
        this.oDataId = oDataId;
    }

    public String getoDataType() {
        return oDataType;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public PowerSupplyType getPowerSupplyType() {
        return powerSupplyType;
    }

    public void setPowerSupplyType(PowerSupplyType powerSupplyType) {
        this.powerSupplyType = powerSupplyType;
    }

    public LineInputVoltageType getLineInputVoltageType() {
        return lineInputVoltageType;
    }

    public void setLineInputVoltageType(LineInputVoltageType lineInputVoltageType) {
        this.lineInputVoltageType = lineInputVoltageType;
    }

    public BigDecimal getLineInputVoltage() {
        return lineInputVoltage;
    }

    public void setLineInputVoltage(BigDecimal lineInputVoltage) {
        this.lineInputVoltage = lineInputVoltage;
    }

    public BigDecimal getPowerCapacityWatts() {
        return powerCapacityWatts;
    }

    public void setPowerCapacityWatts(BigDecimal powerCapacityWatts) {
        this.powerCapacityWatts = powerCapacityWatts;
    }

    public BigDecimal getLastPowerOutputWatts() {
        return lastPowerOutputWatts;
    }

    public void setLastPowerOutputWatts(BigDecimal lastPowerOutputWatts) {
        this.lastPowerOutputWatts = lastPowerOutputWatts;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public String getSparePartNumber() {
        return sparePartNumber;
    }

    public void setSparePartNumber(String sparePartNumber) {
        this.sparePartNumber = sparePartNumber;
    }

    public List<InputRangeDto> getInputRanges() {
        return inputRanges;
    }

    public void setInputRanges(List<InputRangeDto> inputRanges) {
        this.inputRanges = inputRanges;
    }

    public List<Context> getRedundancy() {
        return redundancy;
    }

    public void setRedundancy(List<Context> redundancy) {
        this.redundancy = redundancy;
    }

    public List<Context> getRelatedItem() {
        return relatedItem;
    }

    public void setRelatedItem(List<Context> relatedItem) {
        this.relatedItem = relatedItem;
    }

    public JsonNode getOem() {
        return oem;
    }

    public void setOem(JsonNode oem) {
        this.oem = oem;
    }

    @Override
    public int compareTo(PowerSupplyDto other) {
        return this.getMemberId().compareTo(other.getMemberId());
    }
}
