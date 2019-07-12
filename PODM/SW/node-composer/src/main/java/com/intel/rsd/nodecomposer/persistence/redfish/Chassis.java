/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;

import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "chassis")
@SuppressWarnings({"checkstyle:MethodCount"})
public class Chassis extends DiscoverableEntity {
    @OneToMany(mappedBy = "containedByChassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> containedChassis = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitch> ethernetSwitches = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Storage> storages = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<NetworkAdapter> networkAdapters = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "chassis_computer_system",
        joinColumns = {@JoinColumn(name = "chassis_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "computer_system_id", referencedColumnName = "id")})
    private Set<ComputerSystem> computerSystems = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "chassis_fabric_switch",
        joinColumns = {@JoinColumn(name = "chassis_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "fabric_switch_id", referencedColumnName = "id")})
    private Set<Switch> fabricSwitches = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "chassis_pcie_device",
        joinColumns = {@JoinColumn(name = "chassis_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "pcie_device_id", referencedColumnName = "id")})
    private Set<PcieDevice> pcieDevices = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "parent_chassis_id")
    private Chassis containedByChassis;

    public Set<Chassis> getContainedChassis() {
        return containedChassis;
    }

    public void addContainedChassis(Chassis chassis) {
        requiresNonNull(chassis, "chassis");

        this.containedChassis.add(chassis);
        if (!this.equals(chassis.getContainedByChassis())) {
            chassis.setContainedByChassis(this);
        }
    }

    public void unlinkContainedChassis(Chassis chassis) {
        if (this.containedChassis.contains(chassis)) {
            this.containedChassis.remove(chassis);
            if (chassis != null) {
                chassis.unlinkContainedByChassis(this);
            }
        }
    }

    public Set<EthernetSwitch> getEthernetSwitches() {
        return ethernetSwitches;
    }

    public void addEthernetSwitch(EthernetSwitch ethernetSwitch) {
        requiresNonNull(ethernetSwitch, "ethernetSwitch");

        ethernetSwitches.add(ethernetSwitch);
        if (!this.equals(ethernetSwitch.getChassis())) {
            ethernetSwitch.setChassis(this);
        }
    }

    public void unlinkEthernetSwitch(EthernetSwitch ethernetSwitch) {
        if (ethernetSwitches.contains(ethernetSwitch)) {
            ethernetSwitches.remove(ethernetSwitch);
            if (ethernetSwitch != null) {
                ethernetSwitch.unlinkChassis(this);
            }
        }
    }

    public Set<Drive> getDrives() {
        return drives;
    }

    public void addDrive(Drive drive) {
        requiresNonNull(drive, "drive");

        drives.add(drive);
        if (!this.equals(drive.getChassis())) {
            drive.setChassis(this);
        }
    }

    public void unlinkDrive(Drive drive) {
        if (drives.contains(drive)) {
            drives.remove(drive);
            if (drive != null) {
                drive.unlinkChassis(this);
            }
        }
    }

    public Set<Storage> getStorages() {
        return storages;
    }

    public void addStorage(Storage storage) {
        requiresNonNull(storage, "storage");

        storages.add(storage);
        if (!this.equals(storage.getChassis())) {
            storage.setChassis(this);
        }
    }

    public void unlinkStorage(Storage storage) {
        if (storages.contains(storage)) {
            storages.remove(storage);
            if (storage != null) {
                storage.unlinkChassis(this);
            }
        }
    }

    public Set<ComputerSystem> getComputerSystems() {
        return computerSystems;
    }

    public void addComputerSystem(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "computerSystem");

        computerSystems.add(computerSystem);
        if (!computerSystem.getChassis().contains(this)) {
            computerSystem.addChassis(this);
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (computerSystems.contains(computerSystem)) {
            computerSystems.remove(computerSystem);
            if (computerSystem != null) {
                computerSystem.unlinkChassis(this);
            }
        }
    }

    public Set<Switch> getSwitch() {
        return fabricSwitches;
    }

    public void addSwitch(Switch fabricSwitch) {
        requiresNonNull(fabricSwitch, "switch");

        fabricSwitches.add(fabricSwitch);
        if (!fabricSwitch.getChassis().contains(this)) {
            fabricSwitch.addChassis(this);
        }
    }

    public void unlinkSwitch(Switch fabricSwitch) {
        if (fabricSwitches.contains(fabricSwitch)) {
            fabricSwitches.remove(fabricSwitch);
            if (fabricSwitch != null) {
                fabricSwitch.unlinkChassis(this);
            }
        }
    }

    public Set<PcieDevice> getPcieDevices() {
        return pcieDevices;
    }

    public void addPcieDevice(PcieDevice pcieDevice) {
        requiresNonNull(pcieDevice, "pcieDevice");

        pcieDevices.add(pcieDevice);
        if (!pcieDevice.getChassis().contains(this)) {
            pcieDevice.addChassis(this);
        }
    }

    public void unlinkPcieDevice(PcieDevice pcieDevice) {
        if (pcieDevices.contains(pcieDevice)) {
            pcieDevices.remove(pcieDevice);
            if (pcieDevice != null) {
                pcieDevice.unlinkChassis(this);
            }
        }
    }

    public Chassis getContainedByChassis() {
        return containedByChassis;
    }

    public void setContainedByChassis(Chassis chassis) {
        if (!Objects.equals(this.containedByChassis, chassis)) {
            unlinkContainedByChassis(this.containedByChassis);
            this.containedByChassis = chassis;
            if (chassis != null && !chassis.getContainedChassis().contains(this)) {
                chassis.addContainedChassis(this);
            }
        }
    }

    public void unlinkContainedByChassis(Chassis chassis) {
        if (Objects.equals(this.containedByChassis, chassis)) {
            this.containedByChassis = null;
            if (chassis != null) {
                chassis.unlinkContainedChassis(this);
            }
        }
    }

    public Set<NetworkAdapter> getNetworkAdapters() {
        return networkAdapters;
    }

    public void addNetworkAdapter(NetworkAdapter networkAdapter) {
        requiresNonNull(networkAdapter, "networkAdapter");

        networkAdapters.add(networkAdapter);
        if (!this.equals(networkAdapter.getChassis())) {
            networkAdapter.setChassis(this);
        }
    }

    public void unlinkNetworkAdapter(NetworkAdapter networkAdapter) {
        if (networkAdapters.contains(networkAdapter)) {
            networkAdapters.remove(networkAdapter);
            if (networkAdapter != null) {
                networkAdapter.unlinkChassis(this);
            }
        }
    }

    public Set<ComputerSystem> getAllComputerSystemsUnderneath() {
        return getAllComputerSystemsUnderneathChassis(this, new HashSet<>());
    }

    public Set<Drive> getAllDrivesUnderneath() {
        return getAllDrivesUnderneathChassis(this, new HashSet<>());
    }

    @Override
    public void preRemove() {
        unlinkCollection(containedChassis, this::unlinkContainedChassis);
        unlinkCollection(ethernetSwitches, this::unlinkEthernetSwitch);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(storages, this::unlinkStorage);
        unlinkCollection(computerSystems, this::unlinkComputerSystem);
        unlinkCollection(pcieDevices, this::unlinkPcieDevice);
        unlinkCollection(fabricSwitches, this::unlinkSwitch);
        unlinkCollection(networkAdapters, this::unlinkNetworkAdapter);
        unlinkContainedByChassis(containedByChassis);
    }

    private Set<ComputerSystem> getAllComputerSystemsUnderneathChassis(Chassis chassis, Set<Chassis> visitedChassis) {
        Set<ComputerSystem> computerSystems = new HashSet<>(chassis.getComputerSystems());
        visitedChassis.add(chassis);

        chassis.getContainedChassis().stream()
            .filter(containedChassis -> !visitedChassis.contains(containedChassis))
            .forEach(containedChassis -> computerSystems.addAll(getAllComputerSystemsUnderneathChassis(containedChassis, visitedChassis)));
        return computerSystems;
    }

    private Set<Drive> getAllDrivesUnderneathChassis(Chassis chassis, Set<Chassis> visitedChassis) {
        Set<Drive> drives = new HashSet<>(chassis.getDrives());
        visitedChassis.add(chassis);

        chassis.getContainedChassis().stream()
            .filter(containedChassis -> !visitedChassis.contains(containedChassis))
            .forEach(containedChassis -> drives.addAll(getAllDrivesUnderneathChassis(containedChassis, visitedChassis)));
        return drives;
    }
}
