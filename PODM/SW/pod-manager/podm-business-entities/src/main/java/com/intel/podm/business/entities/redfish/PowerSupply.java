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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.EventOriginProvider;
import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.InputRange;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.LineInputVoltageType;
import com.intel.podm.common.types.PowerSupplyType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "power_supply", indexes = @Index(name = "idx_power_supply_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class PowerSupply extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "member_id")
    private String memberId;

    @Column(name = "power_supply_type")
    @Enumerated(STRING)
    private PowerSupplyType powerSupplyType;

    @Column(name = "line_input_voltage_type")
    @Enumerated(STRING)
    private LineInputVoltageType lineInputVoltageType;

    @Column(name = "line_input_voltage")
    private BigDecimal lineInputVoltage;

    @Column(name = "power_capacity_watts")
    private BigDecimal powerCapacityWatts;

    @Column(name = "last_power_output_watts")
    private BigDecimal lastPowerOutputWatts;

    @Column(name = "model")
    private String model;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "firmware_version")
    private String firmwareVersion;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "spare_part_number")
    private String sparePartNumber;

    @Column(name = "oem")
    private String oem;

    @ElementCollection
    @CollectionTable(name = "power_supply_input_range", joinColumns = @JoinColumn(name = "power_supply_id"))
    @OrderColumn(name = "input_range_order")
    private List<InputRange> inputRanges = new ArrayList<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "power_id")
    private Power power;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
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

    public String getOem() {
        return oem;
    }

    public void setOem(String oem) {
        this.oem = oem;
    }

    public List<InputRange> getInputRanges() {
        return inputRanges;
    }

    public void addInputRange(InputRange inputRange) {
        this.inputRanges.add(inputRange);
    }

    @EventOriginProvider
    public Power getPower() {
        return power;
    }

    public void setPower(Power power) {
        if (!Objects.equals(this.power, power)) {
            unlinkPower(this.power);
            this.power = power;
            if (power != null && !power.getPowerSupplies().contains(this)) {
                power.addPowerSupply(this);
            }
        }
    }

    public void unlinkPower(Power power) {
        if (Objects.equals(this.power, power)) {
            this.power = null;
            if (power != null) {
                power.unlinkPowerSupply(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkPower(power);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, power);
    }
}
