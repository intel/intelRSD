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
import com.intel.podm.business.dto.actions.ResetActionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;

import java.util.HashSet;
import java.util.Set;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "switchType", "model", "manufacturer", "serialNumber", "partNumber",
    "assetTag", "sku", "domainId", "isManaged", "totalSwitchWidth", "indicatorLed", "powerState", "ports", "redundancies", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public final class SwitchDto extends RedfishDto {
    private final Links links = new Links();
    private final Actions actions = new Actions();
    private Status status;
    private Protocol switchType;
    private String model;
    private String manufacturer;
    private String serialNumber;
    private String partNumber;
    private String assetTag;
    @JsonProperty("SKU")
    private String sku;
    @JsonProperty("DomainID")
    private Integer domainId;
    private Boolean isManaged;
    private Integer totalSwitchWidth;
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    private PowerState powerState;
    private SingletonContext ports;
    @JsonProperty("Redundancy")
    private Set<RedundancyDto> redundancies = new HashSet<>();

    public SwitchDto() {
        super("#Switch.v1_0_0.Switch");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Protocol getSwitchType() {
        return switchType;
    }

    public void setSwitchType(Protocol switchType) {
        this.switchType = switchType;
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

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
    }

    public Integer getDomainId() {
        return domainId;
    }

    public void setDomainId(Integer domainId) {
        this.domainId = domainId;
    }

    public Boolean getIsManaged() {
        return isManaged;
    }

    public void setIsManaged(Boolean isManaged) {
        this.isManaged = isManaged;
    }

    public Integer getTotalSwitchWidth() {
        return totalSwitchWidth;
    }

    public void setTotalSwitchWidth(Integer totalSwitchWidth) {
        this.totalSwitchWidth = totalSwitchWidth;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public SingletonContext getPorts() {
        return ports;
    }

    public void setPorts(SingletonContext ports) {
        this.ports = ports;
    }

    public Set<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public void setRedundancies(Set<RedundancyDto> redundancies) {
        this.redundancies = redundancies;
    }

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    @JsonPropertyOrder({"chassis", "managedBy", "oem"})
    public final class Links extends RedfishLinksDto {
        private Set<Context> chassis = new HashSet<>();
        private Set<Context> managedBy = new HashSet<>();

        public Set<Context> getChassis() {
            return chassis;
        }

        public void setChassis(Set<Context> chassis) {
            this.chassis = chassis;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
        }
    }

    @JsonPropertyOrder({"reset", "oem"})
    public class Actions extends RedfishActionsDto {
        @JsonProperty("#Switch.Reset")
        private final ResetActionDto reset = new ResetActionDto();

        public ResetActionDto getReset() {
            return reset;
        }
    }
}
