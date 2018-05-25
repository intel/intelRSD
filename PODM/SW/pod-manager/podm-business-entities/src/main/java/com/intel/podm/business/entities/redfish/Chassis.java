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

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.RackChassisAttributes;
import com.intel.podm.common.types.ChassisType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;

import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
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
@NamedQueries({
    @NamedQuery(name = Chassis.GET_CHASSIS_BY_TYPE,
        query = "SELECT chassis FROM Chassis chassis WHERE chassis.chassisType = :chassisType"),

    @NamedQuery(name = Chassis.GET_CHASSIS_IDS_FROM_PRIMARY_DATA_SOURCE,
        query = "SELECT chassis.entityId FROM Chassis chassis WHERE chassis.isComplementary = false "),

    @NamedQuery(name = Chassis.GET_CHASSIS_BY_TYPE_AND_SERVICE,
        query = "SELECT chassis FROM Chassis chassis JOIN chassis.externalLinks links WHERE links.externalService = :externalService "
            + "AND chassis.chassisType = :chassisType"),

    @NamedQuery(name = Chassis.GET_CHASSIS_MULTI_SOURCE,
        query = "SELECT DISTINCT chas FROM ComputerSystem system JOIN system.chassis chas WHERE chas.chassisType = :type "
            + "AND system.isComplementary = :isComplementary "
            + "AND system.uuid IN (:uuids)"
    ),
    @NamedQuery(name = Chassis.GET_CHASSIS_BY_TYPE_AND_LOCATION,
        query = "SELECT chassis FROM Chassis chassis WHERE chassis.chassisType = :chassisType AND chassis.locationId = :locationId"
    ),
})
@Table(name = "chassis", indexes = @Index(name = "idx_chassis_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class Chassis extends DiscoverableEntity {
    public static final String GET_CHASSIS_BY_TYPE = "GET_CHASSIS_BY_TYPE";
    public static final String GET_CHASSIS_IDS_FROM_PRIMARY_DATA_SOURCE = "GET_CHASSIS_IDS_FROM_PRIMARY_DATA_SOURCE";
    public static final String GET_CHASSIS_MULTI_SOURCE = "GET_CHASSIS_MULTI_SOURCE";
    public static final String GET_CHASSIS_BY_TYPE_AND_SERVICE = "GET_CHASSIS_BY_TYPE_AND_SERVICE";
    public static final String GET_CHASSIS_BY_TYPE_AND_LOCATION = "GET_CHASSIS_BY_TYPE_AND_LOCATION";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "chassis_type")
    @Enumerated(STRING)
    private ChassisType chassisType;

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

    @Column(name = "asset_tag")
    private String assetTag;

    @Column(name = "indicator_led")
    @Enumerated(STRING)
    private IndicatorLed indicatorLed;

    @Column(name = "location_id")
    private String locationId;

    @Column(name = "location_parent_id")
    private String locationParentId;

    @Column(name = "power_state")
    @Enumerated(STRING)
    private PowerState powerState;

    @Embedded
    private RackChassisAttributes rackChassisAttributes;

    @OneToMany(mappedBy = "containedByChassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> containedChassis = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitch> ethernetSwitches = new HashSet<>();

    @OneToMany(mappedBy = "inChassisManager", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Manager> inChassisManagers = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Drive> drives = new HashSet<>();

    @OneToMany(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Storage> storages = new HashSet<>();

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

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "chassis_manager",
        joinColumns = {@JoinColumn(name = "chassis_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "chassis_power",
        joinColumns = {@JoinColumn(name = "chassis_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "power_id", referencedColumnName = "id")})
    private Set<Power> poweredBy = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "chassis_thermal",
        joinColumns = {@JoinColumn(name = "chassis_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "thermal_id", referencedColumnName = "id")})
    private Set<Thermal> cooledBy = new HashSet<>();

    @OneToOne(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Thermal thermal;

    @OneToOne(mappedBy = "chassis", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Power power;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "parent_chassis_id")
    private Chassis containedByChassis;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public ChassisType getChassisType() {
        return chassisType;
    }

    public void setChassisType(ChassisType chassisType) {
        this.chassisType = chassisType;
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

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public String getLocationId() {
        return locationId;
    }

    public void setLocationId(String locationId) {
        this.locationId = locationId;
    }

    public String getLocationParentId() {
        return locationParentId;
    }

    public void setLocationParentId(String locationParentId) {
        this.locationParentId = locationParentId;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public RackChassisAttributes getRackChassisAttributes() {
        return rackChassisAttributes;
    }

    public void setRackChassisAttributes(RackChassisAttributes rackChassisAttributes) {
        this.rackChassisAttributes = rackChassisAttributes;
    }

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

    public Set<Manager> getInChassisManagers() {
        return inChassisManagers;
    }

    public void addInChassisManager(Manager manager) {
        requiresNonNull(manager, "manager");

        this.inChassisManagers.add(manager);
        if (!this.equals(manager.getInChassisManager())) {
            manager.setInChassisManager(this);
        }
    }

    public void unlinkInChassisManager(Manager manager) {
        if (this.inChassisManagers.contains(manager)) {
            this.inChassisManagers.remove(manager);
            if (manager != null) {
                manager.unlinkInChassisManager(this);
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

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getManagedChassis().contains(this)) {
            manager.addManagedChassis(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkManagedChassis(this);
            }
        }
    }

    public Set<Power> getPoweredBy() {
        return poweredBy;
    }

    public void addPoweredBy(Power power) {
        requiresNonNull(power, "power");

        poweredBy.add(power);
        if (!power.getPoweredChassis().contains(this)) {
            power.addPoweredChassis(this);
        }
    }

    public void unlinkPoweredBy(Power power) {
        if (poweredBy.contains(power)) {
            poweredBy.remove(power);
            if (power != null) {
                power.unlinkPoweredChassis(this);
            }
        }
    }

    public Set<Thermal> getCooledBy() {
        return cooledBy;
    }

    public void addCooledBy(Thermal thermal) {
        requiresNonNull(thermal, "thermal");

        cooledBy.add(thermal);
        if (!thermal.getCooledChassis().contains(this)) {
            thermal.addCooledChassis(this);
        }
    }

    public void unlinkCooledBy(Thermal thermal) {
        if (cooledBy.contains(thermal)) {
            cooledBy.remove(thermal);
            if (thermal != null) {
                thermal.unlinkCooledChassis(this);
            }
        }
    }

    public Thermal getThermal() {
        return thermal;
    }

    public void setThermal(Thermal thermal) {
        if (!Objects.equals(this.thermal, thermal)) {
            unlinkThermal(this.thermal);
            this.thermal = thermal;
            if (thermal != null && !this.equals(thermal.getChassis())) {
                thermal.setChassis(this);
            }
        }
    }

    public void unlinkThermal(Thermal thermal) {
        if (Objects.equals(this.thermal, thermal)) {
            this.thermal = null;
            if (thermal != null) {
                thermal.unlinkChassis(this);
            }
        }
    }

    public Power getPower() {
        return power;
    }

    public void setPower(Power power) {
        if (!Objects.equals(this.power, power)) {
            unlinkPower(this.power);
            this.power = power;
            if (power != null && !this.equals(power.getChassis())) {
                power.setChassis(this);
            }
        }
    }

    public void unlinkPower(Power power) {
        if (Objects.equals(this.power, power)) {
            this.power = null;
            if (power != null) {
                power.unlinkChassis(this);
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

    public Set<ComputerSystem> getAllComputerSystemsUnderneath() {
        return getAllComputerSystemsUnderneathChassis(this, new HashSet<>());
    }

    public Set<Drive> getAllDrivesUnderneath() {
        return getAllDrivesUnderneathChassis(this, new HashSet<>());
    }

    public boolean is(ChassisType chassisType) {
        return Objects.equals(getChassisType(), chassisType);
    }

    @Override
    public void preRemove() {
        unlinkCollection(containedChassis, this::unlinkContainedChassis);
        unlinkCollection(ethernetSwitches, this::unlinkEthernetSwitch);
        unlinkCollection(inChassisManagers, this::unlinkInChassisManager);
        unlinkCollection(drives, this::unlinkDrive);
        unlinkCollection(storages, this::unlinkStorage);
        unlinkCollection(computerSystems, this::unlinkComputerSystem);
        unlinkCollection(pcieDevices, this::unlinkPcieDevice);
        unlinkCollection(fabricSwitches, this::unlinkSwitch);
        unlinkCollection(managers, this::unlinkManager);
        unlinkCollection(poweredBy, this::unlinkPoweredBy);
        unlinkCollection(cooledBy, this::unlinkCooledBy);
        unlinkThermal(thermal);
        unlinkPower(power);
        unlinkContainedByChassis(containedByChassis);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, containedByChassis);
    }

    Set<Chassis> getAllOnTopOfChassis() {
        Set<Chassis> chassisSet = new HashSet<>();

        Chassis containedByChassis = getContainedByChassis();
        while (containedByChassis != null && !chassisSet.contains(containedByChassis)) {
            chassisSet.add(containedByChassis);
            containedByChassis = containedByChassis.getContainedByChassis();
        }

        return chassisSet;
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
