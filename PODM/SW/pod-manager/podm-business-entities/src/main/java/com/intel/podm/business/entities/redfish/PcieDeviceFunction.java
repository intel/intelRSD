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
import com.intel.podm.business.entities.redfish.base.NetworkInterfacePossessor;
import com.intel.podm.common.types.DeviceClass;
import com.intel.podm.common.types.FunctionType;
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
@Table(name = "pcie_device_function", indexes = @Index(name = "idx_pcie_device_function_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class PcieDeviceFunction extends DiscoverableEntity implements NetworkInterfacePossessor {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "function_id")
    private Integer functionId;

    @Column(name = "device_class")
    @Enumerated(STRING)
    private DeviceClass deviceClass;

    @Column(name = "function_type")
    @Enumerated(STRING)
    private FunctionType functionType;

    @Column(name = "device_id")
    private String deviceId;

    @Column(name = "vendor_id")
    private String vendorId;

    @Column(name = "class_code")
    private String classCode;

    @Column(name = "revision_id")
    private String revisionId;

    @Column(name = "subsystem_id")
    private String subsystemId;

    @Column(name = "subsystem_vendor_id")
    private String subsystemVendorId;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "pcie_device_id")
    private PcieDevice pcieDevice;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @OneToMany(mappedBy = "pcieDeviceFunction", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @ManyToMany(mappedBy = "pcieDeviceFunctions", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetInterface> ethernetInterfaces = new HashSet<>();

    @ManyToMany(mappedBy = "pcieDeviceFunctions", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StorageController> storageControllers = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public Integer getFunctionId() {
        return functionId;
    }

    public void setFunctionId(Integer functionId) {
        this.functionId = functionId;
    }

    public DeviceClass getDeviceClass() {
        return deviceClass;
    }

    public void setDeviceClass(DeviceClass deviceClass) {
        this.deviceClass = deviceClass;
    }

    public FunctionType getFunctionType() {
        return functionType;
    }

    public void setFunctionType(FunctionType functionType) {
        this.functionType = functionType;
    }

    public String getDeviceId() {
        return deviceId;
    }

    public void setDeviceId(String deviceId) {
        this.deviceId = deviceId;
    }

    public String getVendorId() {
        return vendorId;
    }

    public void setVendorId(String vendorId) {
        this.vendorId = vendorId;
    }

    public String getClassCode() {
        return classCode;
    }

    public void setClassCode(String classCode) {
        this.classCode = classCode;
    }

    public String getRevisionId() {
        return revisionId;
    }

    public void setRevisionId(String revisionId) {
        this.revisionId = revisionId;
    }

    public String getSubsystemId() {
        return subsystemId;
    }

    public void setSubsystemId(String subsystemId) {
        this.subsystemId = subsystemId;
    }

    public String getSubsystemVendorId() {
        return subsystemVendorId;
    }

    public void setSubsystemVendorId(String subsystemVendorId) {
        this.subsystemVendorId = subsystemVendorId;
    }

    @Override
    public Set<EthernetInterface> getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public void addEthernetInterface(EthernetInterface ethernetInterface) {
        requiresNonNull(ethernetInterface, "ethernetInterface");

        ethernetInterfaces.add(ethernetInterface);
        if (!ethernetInterface.getPcieDeviceFunctions().contains(this)) {
            ethernetInterface.addPcieDeviceFunction(this);
        }
    }

    public void unlinkEthernetInterface(EthernetInterface ethernetInterface) {
        if (ethernetInterfaces.contains(ethernetInterface)) {
            ethernetInterfaces.remove(ethernetInterface);
            if (ethernetInterface != null) {
                ethernetInterface.unlinkPcieDeviceFunction(this);
            }
        }
    }

    public Set<StorageController> getStorageControllers() {
        return storageControllers;
    }

    public void addStorageController(StorageController storageController) {
        requiresNonNull(storageController, "storageController");

        storageControllers.add(storageController);
        if (!storageController.getPcieDeviceFunctions().contains(this)) {
            storageController.addPcieDeviceFunction(this);
        }
    }

    public void unlinkStorageController(StorageController storageController) {
        if (storageControllers.contains(storageController)) {
            storageControllers.remove(storageController);
            if (storageController != null) {
                storageController.unlinkPcieDeviceFunction(this);
            }
        }
    }

    public PcieDevice getPcieDevice() {
        return pcieDevice;
    }

    public void setPcieDevice(PcieDevice pcieDevice) {
        if (!Objects.equals(this.pcieDevice, pcieDevice)) {
            unlinkPcieDevice(this.pcieDevice);
            this.pcieDevice = pcieDevice;
            if (pcieDevice != null && !pcieDevice.getPcieDeviceFunctions().contains(this)) {
                pcieDevice.addPcieDeviceFunction(this);
            }
        }
    }

    public void unlinkPcieDevice(PcieDevice pcieDevice) {
        if (Objects.equals(this.pcieDevice, pcieDevice)) {
            this.pcieDevice = null;
            if (pcieDevice != null) {
                pcieDevice.unlinkPcieDeviceFunction(this);
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
            if (!computerSystem.getPcieDeviceFunctions().contains(this)) {
                computerSystem.addPcieDeviceFunction(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkPcieDeviceFunction(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getPcieDeviceFunction())) {
            drive.setPcieDeviceFunction(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkPcieDeviceFunction(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkPcieDevice(pcieDevice);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(ethernetInterfaces, this::unlinkEthernetInterface);
        unlinkCollection(storageControllers, this::unlinkStorageController);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, pcieDevice);
    }
}
