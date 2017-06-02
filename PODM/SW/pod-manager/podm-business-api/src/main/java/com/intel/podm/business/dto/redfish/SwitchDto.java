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

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount", "checkstyle:ClassFanOutComplexity", "checkstyle:MethodLength"})
public final class SwitchDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final Protocol switchType;
    private final String model;
    private final String manufacturer;
    private final String sku;
    private final String serialNumber;
    private final String partNumber;
    private final String assetTag;
    private final IndicatorLed indicatorLed;
    private final Integer domainId;
    private final Boolean isManaged;
    private final Integer totalSwitchWidth;
    private final PowerState powerState;
    private final List<ResetType> allowableResetTypes;
    private final Set<RedundancyDto> redundancies;
    private final Links links;

    private SwitchDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        indicatorLed = builder.indicatorLed;
        model = builder.model;
        manufacturer = builder.manufacturer;
        sku = builder.sku;
        switchType = builder.switchType;
        domainId = builder.domainId;
        isManaged = builder.isManaged;
        serialNumber = builder.serialNumber;
        partNumber = builder.partNumber;
        assetTag = builder.assetTag;
        status = builder.status;
        totalSwitchWidth = builder.totalSwitchWidth;
        powerState = builder.powerState;
        redundancies = builder.redundancies;
        allowableResetTypes = builder.allowableResetTypes;
        links = new Links(builder.chassis, builder.managedBy);
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

    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public Protocol getSwitchType() {
        return switchType;
    }

    public Integer getDomainId() {
        return domainId;
    }

    public Boolean getManaged() {
        return isManaged;
    }

    public Integer getTotalSwitchWidth() {
        return totalSwitchWidth;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public Set<RedundancyDto> getRedundancies() {
        return redundancies;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public String getModel() {
        return model;
    }

    public String getManufacturer() {
        return manufacturer;
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

    public Status getStatus() {
        return status;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> chassis;
        private final Set<Context> managedBy;
        private Object oem;

        public Links(Set<Context> chassis, Set<Context> managedBy) {
            this.chassis = chassis;
            this.managedBy = managedBy;
        }

        public Set<Context> getChassis() {
            return chassis;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private IndicatorLed indicatorLed;
        private String model;
        private String manufacturer;
        private String sku;
        private String serialNumber;
        private String partNumber;
        private String assetTag;
        private Status status;
        private Integer domainId;
        private Boolean isManaged;
        private Integer totalSwitchWidth;
        private PowerState powerState;
        private Protocol switchType;
        private Set<RedundancyDto> redundancies;
        private Set<Context> chassis;
        private Set<Context> managedBy;
        private List<ResetType> allowableResetTypes;

        private Builder() {
        }

        public SwitchDto build() {
            return new SwitchDto(this);
        }

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

        public Builder indicatorLed(IndicatorLed indicatorLed) {
            this.indicatorLed = indicatorLed;
            return this;
        }

        public Builder model(String model) {
            this.model = model;
            return this;
        }

        public Builder manufacturer(String manufacturer) {
            this.manufacturer = manufacturer;
            return this;
        }

        public Builder sku(String sku) {
            this.sku = sku;
            return this;
        }

        public Builder serialNumber(String serialNumber) {
            this.serialNumber = serialNumber;
            return this;
        }

        public Builder partNumber(String partNumber) {
            this.partNumber = partNumber;
            return this;
        }

        public Builder assetTag(String assetTag) {
            this.assetTag = assetTag;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder domainId(Integer domainId) {
            this.domainId = domainId;
            return this;
        }

        public Builder isManaged(Boolean isManaged) {
            this.isManaged = isManaged;
            return this;
        }

        public Builder totalSwitchWidth(Integer totalSwitchWidth) {
            this.totalSwitchWidth = totalSwitchWidth;
            return this;
        }

        public Builder redundancies(Set<RedundancyDto> redundancies) {
            this.redundancies = redundancies;
            return this;
        }

        public Builder chassis(Set<Context> chassis) {
            this.chassis = chassis;
            return this;
        }

        public Builder managedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
            return this;
        }

        public Builder switchType(Protocol switchType) {
            this.switchType = switchType;
            return this;
        }

        public Builder powerState(PowerState powerState) {
            this.powerState = powerState;
            return this;
        }

        public Builder allowableResetTypes(List<ResetType> allowableResetTypes) {
            this.allowableResetTypes = allowableResetTypes;
            return this;
        }
    }
}
