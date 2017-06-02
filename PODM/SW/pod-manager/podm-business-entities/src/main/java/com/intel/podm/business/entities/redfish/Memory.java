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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MemoryModule;
import com.intel.podm.business.entities.redfish.embeddables.MemoryLocation;
import com.intel.podm.business.entities.redfish.embeddables.Region;
import com.intel.podm.common.types.BaseModuleType;
import com.intel.podm.common.types.ErrorCorrection;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.MemoryMedia;
import com.intel.podm.common.types.MemoryType;
import com.intel.podm.common.types.OperatingMemoryMode;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
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
@Table(name = "memory", indexes = @Index(name = "idx_memory_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class Memory extends DiscoverableEntity implements MemoryModule {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "memory_type")
    @Enumerated(STRING)
    private MemoryType memoryType;

    @Column(name = "memory_device_type")
    @Enumerated(STRING)
    private MemoryDeviceType memoryDeviceType;

    @Column(name = "base_module_type")
    @Enumerated(STRING)
    private BaseModuleType baseModuleType;

    @Column(name = "capacity_mib")
    private Integer capacityMib;

    @Column(name = "data_width_bits")
    private Integer dataWidthBits;

    @Column(name = "bus_width_bits")
    private Integer busWidthBits;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "firmware_revision")
    private String firmwareRevision;

    @Column(name = "firmware_api_version")
    private String firmwareApiVersion;

    @Column(name = "vendor_id")
    private String vendorId;

    @Column(name = "device_id")
    private String deviceId;

    @Column(name = "rank_count")
    private Integer rankCount;

    @Column(name = "device_locator")
    private String deviceLocator;

    @Column(name = "error_correction")
    @Enumerated(STRING)
    private ErrorCorrection errorCorrection;

    @Column(name = "operating_speed_mhz")
    private Integer operatingSpeedMhz;

    @Column(name = "voltage_volt")
    private BigDecimal voltageVolt;

    @Embedded
    private MemoryLocation memoryLocation;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "memory_memory_media", joinColumns = @JoinColumn(name = "memory_id"))
    @Column(name = "memory_media")
    @OrderColumn(name = "memory_media_order")
    private List<MemoryMedia> memoryMedia = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "memory_allowed_speed_mhz", joinColumns = @JoinColumn(name = "computer_system_id"))
    @Column(name = "allowed_speed_mhz")
    @OrderColumn(name = "allowed_speed_mhz_order")
    private List<Integer> allowedSpeedsMhz = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "memory_function_class", joinColumns = @JoinColumn(name = "memory_id"))
    @Column(name = "function_class")
    @OrderColumn(name = "function_class_order")
    private List<String> functionClasses = new ArrayList<>();

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "memory_operating_memory_mode", joinColumns = @JoinColumn(name = "memory_id"))
    @Column(name = "operating_memory_mode")
    @OrderColumn(name = "operating_memory_mode_order")
    private List<OperatingMemoryMode> operatingMemoryModes = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "memory_region", joinColumns = @JoinColumn(name = "memory_id"))
    @OrderColumn(name = "memory_region_order")
    private List<Region> regions = new ArrayList<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public MemoryType getMemoryType() {
        return memoryType;
    }

    public void setMemoryType(MemoryType memoryType) {
        this.memoryType = memoryType;
    }

    @Override
    public MemoryDeviceType getMemoryDeviceType() {
        return memoryDeviceType;
    }

    public void setMemoryDeviceType(MemoryDeviceType memoryDeviceType) {
        this.memoryDeviceType = memoryDeviceType;
    }

    public BaseModuleType getBaseModuleType() {
        return baseModuleType;
    }

    public void setBaseModuleType(BaseModuleType baseModuleType) {
        this.baseModuleType = baseModuleType;
    }

    @Override
    public Integer getCapacityMib() {
        return capacityMib;
    }

    public void setCapacityMib(Integer capacityMib) {
        this.capacityMib = capacityMib;
    }

    @Override
    public Integer getDataWidthBits() {
        return dataWidthBits;
    }

    public void setDataWidthBits(Integer dataWidthBits) {
        this.dataWidthBits = dataWidthBits;
    }

    public Integer getBusWidthBits() {
        return busWidthBits;
    }

    public void setBusWidthBits(Integer busWidthBits) {
        this.busWidthBits = busWidthBits;
    }

    @Override
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

    public String getFirmwareRevision() {
        return firmwareRevision;
    }

    public void setFirmwareRevision(String firmwareRevision) {
        this.firmwareRevision = firmwareRevision;
    }

    public String getFirmwareApiVersion() {
        return firmwareApiVersion;
    }

    public void setFirmwareApiVersion(String firmwareApiVersion) {
        this.firmwareApiVersion = firmwareApiVersion;
    }

    public String getVendorId() {
        return vendorId;
    }

    public void setVendorId(String vendorId) {
        this.vendorId = vendorId;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public Integer getRankCount() {
        return rankCount;
    }

    public void setRankCount(Integer rankCount) {
        this.rankCount = rankCount;
    }

    public String getDeviceLocator() {
        return deviceLocator;
    }

    public void setDeviceLocator(String deviceLocator) {
        this.deviceLocator = deviceLocator;
    }

    public ErrorCorrection getErrorCorrection() {
        return errorCorrection;
    }

    public void setErrorCorrection(ErrorCorrection errorCorrection) {
        this.errorCorrection = errorCorrection;
    }

    @Override
    public Integer getOperatingSpeedMhz() {
        return operatingSpeedMhz;
    }

    public void setOperatingSpeedMhz(Integer operatingSpeedMhz) {
        this.operatingSpeedMhz = operatingSpeedMhz;
    }

    public BigDecimal getVoltageVolt() {
        return voltageVolt;
    }

    public void setVoltageVolt(BigDecimal voltageVolt) {
        this.voltageVolt = voltageVolt;
    }

    public MemoryLocation getMemoryLocation() {
        return memoryLocation;
    }

    public void setMemoryLocation(MemoryLocation memoryLocation) {
        this.memoryLocation = memoryLocation;
    }

    public List<MemoryMedia> getMemoryMedia() {
        return memoryMedia;
    }

    public void addMemoryMedia(MemoryMedia memoryMedia) {
        this.memoryMedia.add(memoryMedia);
    }

    public List<Integer> getAllowedSpeedsMhz() {
        return allowedSpeedsMhz;
    }

    public void addAllowedSpeedMhz(Integer allowedSpeed) {
        this.allowedSpeedsMhz.add(allowedSpeed);
    }

    public List<String> getFunctionClasses() {
        return functionClasses;
    }

    public void addFunctionClass(String functionClass) {
        this.functionClasses.add(functionClass);
    }

    public List<OperatingMemoryMode> getOperatingMemoryModes() {
        return operatingMemoryModes;
    }

    public void addOperatingMemoryMode(OperatingMemoryMode operatingMemoryMode) {
        this.operatingMemoryModes.add(operatingMemoryMode);
    }

    public List<Region> getRegions() {
        return regions;
    }

    public void addRegion(Region region) {
        regions.add(region);
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && !computerSystem.getMemoryModules().contains(this)) {
                computerSystem.addMemoryModule(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkMemoryModule(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, computerSystem);
    }
}
