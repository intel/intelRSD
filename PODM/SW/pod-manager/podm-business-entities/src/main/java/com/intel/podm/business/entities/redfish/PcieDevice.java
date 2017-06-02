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
import com.intel.podm.common.types.DeviceType;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "pcie_device", indexes = @Index(name = "idx_pcie_device_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class PcieDevice extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "asset_tag")
    private String assetTag;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model")
    private String model;

    @Column(name = "sku")
    private String sku;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "device_type")
    @Enumerated(STRING)
    private DeviceType deviceType;

    @Column(name = "firmware_version")
    private String firmwareVersion;

    @OneToMany(mappedBy = "pcieDevice", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PcieDeviceFunction> pcieDeviceFunctions = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @ManyToMany(mappedBy = "pcieDevices", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> chassis = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public String getManufacturer() {
        return manufacturer;
    }

    public void setManufacturer(String manufacturer) {
        this.manufacturer = manufacturer;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
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

    public DeviceType getDeviceType() {
        return deviceType;
    }

    public void setDeviceType(DeviceType deviceType) {
        this.deviceType = deviceType;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public Set<PcieDeviceFunction> getPcieDeviceFunctions() {
        return pcieDeviceFunctions;
    }

    public void addPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        requiresNonNull(pcieDeviceFunction, "pcieDeviceFunction");

        pcieDeviceFunctions.add(pcieDeviceFunction);
        if (!this.equals(pcieDeviceFunction.getPcieDevice())) {
            pcieDeviceFunction.setPcieDevice(this);
        }
    }

    public void unlinkPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        if (pcieDeviceFunctions.contains(pcieDeviceFunction)) {
            pcieDeviceFunctions.remove(pcieDeviceFunction);
            if (pcieDeviceFunction != null) {
                pcieDeviceFunction.unlinkPcieDevice(this);
            }
        }
    }

    public Set<Chassis> getChassis() {
        return chassis;
    }

    public void addChassis(Chassis newChassis) {
        requiresNonNull(newChassis, "newChassis");

        chassis.add(newChassis);
        if (!newChassis.getPcieDevices().contains(this)) {
            newChassis.addPcieDevice(this);
        }
    }

    public void unlinkChassis(Chassis chassisToUnlink) {
        if (chassis.contains(chassisToUnlink)) {
            chassis.remove(chassisToUnlink);
            if (chassisToUnlink != null) {
                chassisToUnlink.unlinkPcieDevice(this);
            }
        }
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (computerSystem == null) {
            unlinkComputerSystem(this.computerSystem);
        } else {
            this.computerSystem = computerSystem;
            if (!computerSystem.getPcieDevices().contains(this)) {
                computerSystem.addPcieDevice(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkPcieDevice(this);
            }
        }
    }


    @Override
    public void preRemove() {
        unlinkCollection(pcieDeviceFunctions, this::unlinkPcieDeviceFunction);
        unlinkCollection(chassis, this::unlinkChassis);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }
}
