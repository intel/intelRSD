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
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.NetworkInterfacePossessor;
import com.intel.podm.business.entities.redfish.embeddables.CommandShell;
import com.intel.podm.business.entities.redfish.embeddables.GraphicalConsole;
import com.intel.podm.business.entities.redfish.embeddables.SerialConsole;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.ManagerType;
import com.intel.podm.common.types.PowerState;

import javax.persistence.AttributeOverride;
import javax.persistence.AttributeOverrides;
import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "manager", indexes = @Index(name = "idx_manager_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class Manager extends DiscoverableEntity implements NetworkInterfacePossessor {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "manager_type")
    @Enumerated(STRING)
    private ManagerType managerType;

    @Column(name = "service_entry_point_uuid")
    private UUID serviceEntryPointUuid;

    @Column(name = "uuid")
    private UUID uuid;

    @Column(name = "model")
    private String model;

    @Column(name = "firmware_version")
    private String firmwareVersion;

    @Column(name = "power_state")
    @Enumerated(STRING)
    private PowerState powerState;

    @SuppressEvents
    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetInterface> ethernetInterfaces = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ComputerSystem> computerSystems = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Switch> fabricSwitches = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> managedChassis = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitch> ethernetSwitches = new HashSet<>();

    @ManyToMany(mappedBy = "managers", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StorageService> storageServices = new HashSet<>();

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "serviceEnabled", column = @Column(name = "graphical_console_service_enabled")),
        @AttributeOverride(name = "maxConcurrentSessions", column = @Column(name = "graphical_console_max_concurrent_sessions"))
    })
    private GraphicalConsole graphicalConsole;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "serviceEnabled", column = @Column(name = "serial_console_service_enabled")),
        @AttributeOverride(name = "maxConcurrentSessions", column = @Column(name = "serial_console_max_concurrent_sessions"))
    })
    private SerialConsole serialConsole;

    @Embedded
    @AttributeOverrides({
        @AttributeOverride(name = "serviceEnabled", column = @Column(name = "command_shell_service_enabled")),
        @AttributeOverride(name = "maxConcurrentSessions", column = @Column(name = "command_shell_max_concurrent_sessions"))
    })
    private CommandShell commandShell;

    @OneToOne(mappedBy = "manager", fetch = LAZY, cascade = {MERGE, PERSIST})
    private NetworkProtocol networkProtocol;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "in_chassis_manager_id")
    private Chassis inChassisManager;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public ManagerType getManagerType() {
        return managerType;
    }

    public void setManagerType(ManagerType managerType) {
        this.managerType = managerType;
    }

    public UUID getServiceEntryPointUuid() {
        return serviceEntryPointUuid;
    }

    public void setServiceEntryPointUuid(UUID serviceEntryPointUuid) {
        this.serviceEntryPointUuid = serviceEntryPointUuid;
    }

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public String getModel() {
        return model;
    }

    public void setModel(String model) {
        this.model = model;
    }

    public String getFirmwareVersion() {
        return firmwareVersion;
    }

    public void setFirmwareVersion(String firmwareVersion) {
        this.firmwareVersion = firmwareVersion;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    @Override
    public Set<EthernetInterface> getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public void addEthernetInterface(EthernetInterface ethernetInterface) {
        requiresNonNull(ethernetInterface, "ethernetInterface");

        ethernetInterfaces.add(ethernetInterface);
        if (!ethernetInterface.getManagers().contains(this)) {
            ethernetInterface.addManager(this);
        }
    }

    public void unlinkEthernetInterface(EthernetInterface ethernetInterface) {
        if (ethernetInterfaces.contains(ethernetInterface)) {
            ethernetInterfaces.remove(ethernetInterface);
            if (ethernetInterface != null) {
                ethernetInterface.unlinkManager(this);
            }
        }
    }

    public Set<ComputerSystem> getComputerSystems() {
        return computerSystems;
    }

    public void addComputerSystem(ComputerSystem computerSystem) {
        requiresNonNull(computerSystem, "computerSystem");

        computerSystems.add(computerSystem);
        if (!computerSystem.getManagers().contains(this)) {
            computerSystem.addManager(this);
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (computerSystems.contains(computerSystem)) {
            computerSystems.remove(computerSystem);
            if (computerSystem != null) {
                computerSystem.unlinkManager(this);
            }
        }
    }

    public Set<Chassis> getManagedChassis() {
        return managedChassis;
    }

    public void addManagedChassis(Chassis chassis) {
        requiresNonNull(chassis, "chassis");

        managedChassis.add(chassis);
        if (!chassis.getManagers().contains(this)) {
            chassis.addManager(this);
        }
    }

    public void unlinkManagedChassis(Chassis chassis) {
        if (managedChassis.contains(chassis)) {
            managedChassis.remove(chassis);
            if (chassis != null) {
                chassis.unlinkManager(this);
            }
        }
    }

    public Set<Switch> getSwitches() {
        return fabricSwitches;
    }

    public void addSwitch(Switch fabricSwitch) {
        requiresNonNull(fabricSwitch, "fabricSwitch");

        fabricSwitches.add(fabricSwitch);
        if (!fabricSwitch.getManagers().contains(this)) {
            fabricSwitch.addManager(this);
        }
    }

    public void unlinkSwitch(Switch fabricSwitch) {
        if (fabricSwitches.contains(fabricSwitch)) {
            fabricSwitches.remove(fabricSwitch);
            if (fabricSwitch != null) {
                fabricSwitch.unlinkManager(this);
            }
        }
    }

    public Set<EthernetSwitch> getEthernetSwitches() {
        return ethernetSwitches;
    }

    public void addEthernetSwitch(EthernetSwitch ethernetSwitch) {
        requiresNonNull(ethernetSwitch, "ethernetSwitch");

        ethernetSwitches.add(ethernetSwitch);
        if (!ethernetSwitch.getManagers().contains(this)) {
            ethernetSwitch.addManager(this);
        }
    }

    public void unlinkEthernetSwitch(EthernetSwitch ethernetSwitch) {
        if (ethernetSwitches.contains(ethernetSwitch)) {
            ethernetSwitches.remove(ethernetSwitch);
            if (ethernetSwitch != null) {
                ethernetSwitch.unlinkManager(this);
            }
        }
    }

    public Set<StorageService> getStorageServices() {
        return storageServices;
    }

    public void addStorageService(StorageService storageService) {
        requiresNonNull(storageService, "storageService");

        storageServices.add(storageService);
        if (!storageService.getManagers().contains(this)) {
            storageService.addManager(this);
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (storageServices.contains(storageService)) {
            storageServices.remove(storageService);
            if (storageService != null) {
                storageService.unlinkManager(this);
            }
        }
    }

    public GraphicalConsole getGraphicalConsole() {
        return graphicalConsole;
    }

    public void setGraphicalConsole(GraphicalConsole graphicalConsole) {
        this.graphicalConsole = graphicalConsole;
    }

    public SerialConsole getSerialConsole() {
        return serialConsole;
    }

    public void setSerialConsole(SerialConsole serialConsole) {
        this.serialConsole = serialConsole;
    }

    public CommandShell getCommandShell() {
        return commandShell;
    }

    public void setCommandShell(CommandShell commandShell) {
        this.commandShell = commandShell;
    }

    public NetworkProtocol getNetworkProtocol() {
        return networkProtocol;
    }

    public void setNetworkProtocol(NetworkProtocol networkProtocol) {
        if (!Objects.equals(this.networkProtocol, networkProtocol)) {
            unlinkNetworkProtocol(this.networkProtocol);
            this.networkProtocol = networkProtocol;
            if (networkProtocol != null && !this.equals(networkProtocol.getManager())) {
                networkProtocol.setManager(this);
            }
        }
    }

    public void unlinkNetworkProtocol(NetworkProtocol networkProtocol) {
        if (Objects.equals(this.networkProtocol, networkProtocol)) {
            this.networkProtocol = null;
            if (networkProtocol != null) {
                networkProtocol.unlinkManager(this);
            }
        }
    }

    public Chassis getInChassisManager() {
        return inChassisManager;
    }

    public void setInChassisManager(Chassis chassis) {
        if (!Objects.equals(this.inChassisManager, chassis)) {
            unlinkInChassisManager(this.inChassisManager);
            this.inChassisManager = chassis;
            if (chassis != null && !chassis.getInChassisManagers().contains(this)) {
                chassis.addInChassisManager(this);
            }
        }
    }

    public void unlinkInChassisManager(Chassis chassis) {
        if (Objects.equals(this.inChassisManager, chassis)) {
            this.inChassisManager = null;
            if (chassis != null) {
                chassis.unlinkInChassisManager(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(ethernetInterfaces, this::unlinkEthernetInterface);
        unlinkCollection(computerSystems, this::unlinkComputerSystem);
        unlinkCollection(managedChassis, this::unlinkManagedChassis);
        unlinkCollection(ethernetSwitches, this::unlinkEthernetSwitch);
        unlinkCollection(storageServices, this::unlinkStorageService);
        unlinkNetworkProtocol(networkProtocol);
        unlinkInChassisManager(inChassisManager);
        unlinkCollection(fabricSwitches, this::unlinkSwitch);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
