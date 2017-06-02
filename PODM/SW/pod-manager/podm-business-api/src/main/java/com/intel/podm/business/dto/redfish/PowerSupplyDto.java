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

import com.intel.podm.business.dto.redfish.attributes.InputRangeDto;
import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PowerSupplyType;
import com.intel.podm.common.types.Status;

import java.math.BigDecimal;
import java.util.List;

@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount"})
public final class PowerSupplyDto {
    private final Context context;
    private final String memberId;
    private final String name;
    private final Status status;
    private final PowerSupplyType powerSupplyType;
    private final LineInputVoltageType lineInputVoltageType;
    private final BigDecimal lineInputVoltage;
    private final BigDecimal powerCapacityWatts;
    private final BigDecimal lastPowerOutputWatts;
    private final String model;
    private final String manufacturer;
    private final String firmwareVersion;
    private final String serialNumber;
    private final String partNumber;
    private final String sparePartNumber;
    private final List<InputRangeDto> inputRanges;
    private final IndicatorLed indicatorLed;
    private final List<Context> redundancy;
    private final List<Context> relatedItem;
    private final UnknownOemDto oem;

    private PowerSupplyDto(Builder builder) {
        this.context = builder.context;
        this.memberId = builder.memberId;
        this.name = builder.name;
        this.status = builder.status;
        this.powerSupplyType = builder.powerSupplyType;
        this.lineInputVoltageType = builder.lineInputVoltageType;
        this.lineInputVoltage = builder.lineInputVoltage;
        this.powerCapacityWatts = builder.powerCapacityWatts;
        this.lastPowerOutputWatts = builder.lastPowerOutputWatts;
        this.model = builder.model;
        this.manufacturer = builder.manufacturer;
        this.firmwareVersion = builder.firmwareVersion;
        this.serialNumber = builder.serialNumber;
        this.partNumber = builder.partNumber;
        this.sparePartNumber = builder.sparePartNumber;
        this.inputRanges = builder.inputRanges;
        this.indicatorLed = builder.indicatorLed;
        this.redundancy = builder.redundancy;
        this.relatedItem = builder.relatedItem;
        this.oem = builder.oem;
    }

    public Context getContext() {
        return context;
    }

    public String getMemberId() {
        return memberId;
    }

    public String getName() {
        return name;
    }

    public Status getStatus() {
        return status;
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

    public List<InputRangeDto> getInputRanges() {
        return inputRanges;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public List<Context> getRedundancy() {
        return redundancy;
    }

    public List<Context> getRelatedItem() {
        return relatedItem;
    }

    public UnknownOemDto getOem() {
        return oem;
    }

    public static Builder newPowerSupplyDto() {
        return new Builder();
    }

    public static final class Builder {
        private Context context;
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
        private List<InputRangeDto> inputRanges;
        private IndicatorLed indicatorLed;
        private List<Context> redundancy;
        private List<Context> relatedItem;
        private UnknownOemDto oem;

        private Builder() { }

        public PowerSupplyDto build() {
            return new PowerSupplyDto(this);
        }

        public Builder context(Context context) {
            this.context = context;
            return this;
        }

        public Builder memberId(String memberId) {
            this.memberId = memberId;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder powerSupplyType(PowerSupplyType powerSupplyType) {
            this.powerSupplyType = powerSupplyType;
            return this;
        }

        public Builder lineInputVoltageType(LineInputVoltageType lineInputVoltageType) {
            this.lineInputVoltageType = lineInputVoltageType;
            return this;
        }

        public Builder lineInputVoltage(BigDecimal lineInputVoltage) {
            this.lineInputVoltage = lineInputVoltage;
            return this;
        }

        public Builder powerCapacityWatts(BigDecimal powerCapacityWatts) {
            this.powerCapacityWatts = powerCapacityWatts;
            return this;
        }

        public Builder lastPowerOutputWatts(BigDecimal lastPowerOutputWatts) {
            this.lastPowerOutputWatts = lastPowerOutputWatts;
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

        public Builder firmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
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

        public Builder sparePartNumber(String sparePartNumber) {
            this.sparePartNumber = sparePartNumber;
            return this;
        }

        public Builder inputRanges(List<InputRangeDto> inputRanges) {
            this.inputRanges = inputRanges;
            return this;
        }

        public Builder indicatorLed(IndicatorLed indicatorLed) {
            this.indicatorLed = indicatorLed;
            return this;
        }

        public Builder redundancy(List<Context> redundancy) {
            this.redundancy = redundancy;
            return this;
        }

        public Builder relatedItem(List<Context> relatedItem) {
            this.relatedItem = relatedItem;
            return this;
        }

        public Builder oem(UnknownOemDto oem) {
            this.oem = oem;
            return this;
        }
    }
}
