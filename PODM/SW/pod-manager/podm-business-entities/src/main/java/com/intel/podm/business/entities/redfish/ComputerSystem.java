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

import com.intel.podm.business.entities.EventRedirectionSource;
import com.intel.podm.business.entities.EventRedirectionTarget;
import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.listeners.ComputerSystemListener;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MemoryModule;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.business.entities.redfish.base.NetworkInterfacePossessor;
import com.intel.podm.business.entities.redfish.base.Resettable;
import com.intel.podm.business.entities.redfish.embeddables.Boot;
import com.intel.podm.business.entities.redfish.embeddables.ComputerSystemDevice;
import com.intel.podm.business.entities.redfish.embeddables.MemorySummary;
import com.intel.podm.business.entities.redfish.embeddables.ProcessorSummary;
import com.intel.podm.business.entities.redfish.embeddables.TrustedModule;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "computer_system", indexes = @Index(name = "idx_computer_system_entity_id", columnList = "entity_id", unique = true))
@NamedQueries({
    @NamedQuery(name = ComputerSystem.GET_COMPUTER_SYSTEM_IDS_FROM_PRIMARY_DATA_SOURCE,
        query = ComputerSystemQueries.GET_COMPUTER_SYSTEM_IDS_FROM_PRIMARY_DATA_SOURCE),
    @NamedQuery(name = ComputerSystem.GET_PRIMARY_COMPUTER_SYSTEM, query = ComputerSystemQueries.GET_PRIMARY_COMPUTER_SYSTEM),
    @NamedQuery(name = ComputerSystem.GET_COMPUTER_SYSTEM_MULTI_SOURCE, query = ComputerSystemQueries.GET_COMPUTER_SYSTEM_MULTI_SOURCE),
    @NamedQuery(name = ComputerSystem.GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE, query = ComputerSystemQueries.GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE),
    @NamedQuery(name = ComputerSystem.GET_COMPUTER_SYSTEMS_MATCHING_CONNECTION_ID, query = ComputerSystemQueries.GET_COMPUTER_SYSTEMS_MATCHING_CONNECTION_ID)
})
@EntityListeners(ComputerSystemListener.class)
@Eventable
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class ComputerSystem extends DiscoverableEntity implements NetworkInterfacePossessor, MultiSourceResource, Resettable {
    public static final String GET_COMPUTER_SYSTEM_IDS_FROM_PRIMARY_DATA_SOURCE = "GET_COMPUTER_SYSTEM_IDS_FROM_PRIMARY_DATA_SOURCE";
    public static final String GET_PRIMARY_COMPUTER_SYSTEM = "GET_PRIMARY_COMPUTER_SYSTEM";
    public static final String GET_COMPUTER_SYSTEM_MULTI_SOURCE = "GET_COMPUTER_SYSTEM_MULTI_SOURCE";
    public static final String GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE = "GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE";
    public static final String GET_COMPUTER_SYSTEMS_MATCHING_CONNECTION_ID = "GET_COMPUTER_SYSTEMS_MATCHING_CONNECTION_ID";
    private static final String ENTITY_NAME = "ComputerSystem";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "manufacturer")
    private String manufacturer;

    @Column(name = "model")
    private String model;

    @Column(name = "serial_number")
    private String serialNumber;

    @Column(name = "uuid")
    private UUID uuid;

    @Column(name = "system_type")
    @Enumerated(STRING)
    private SystemType systemType;

    @Column(name = "asset_tag")
    private String assetTag;

    @Column(name = "bios_version")
    private String biosVersion;

    @Column(name = "sku")
    private String sku;

    @Column(name = "host_name")
    private String hostName;

    @Column(name = "indicator_led")
    @Enumerated(STRING)
    private IndicatorLed indicatorLed;

    @Column(name = "memory_sockets")
    private Integer memorySockets;

    @Column(name = "processor_sockets")
    private Integer processorSockets;

    @Column(name = "discovery_state")
    @Enumerated(STRING)
    private DiscoveryState discoveryState;

    @Column(name = "power_state")
    @Enumerated(STRING)
    @EventRedirectionSource
    private PowerState powerState;

    @Column(name = "part_number")
    private String partNumber;

    @Column(name = "user_mode_enabled")
    private Boolean userModeEnabled;

    @Column(name = "trusted_execution_technology_enabled")
    private Boolean trustedExecutionTechnologyEnabled;

    @EventRedirectionSource
    @Embedded
    private Boot boot;

    @Embedded
    @EventRedirectionSource
    private ProcessorSummary processorSummary;

    @Embedded
    private MemorySummary memorySummary;

    @ElementCollection
    @CollectionTable(name = "computer_system_pcie_connection_id", joinColumns = @JoinColumn(name = "computer_system_id"))
    @Column(name = "pcie_connection_id")
    @OrderColumn(name = "pcie_connection_id_order")
    private List<String> pcieConnectionIds = new ArrayList<>();

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "computer_system_allowable_reset_type", joinColumns = @JoinColumn(name = "computer_system_id"))
    @Column(name = "allowable_reset_type")
    @OrderColumn(name = "allowable_reset_type_order")
    private List<ResetType> allowableResetTypes = new ArrayList<>();

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "computer_system_allowable_interface_type", joinColumns = @JoinColumn(name = "computer_system_id"))
    @Column(name = "allowable_interface_type")
    @OrderColumn(name = "allowable_interface_type_order")
    private List<InterfaceType> allowableInterfaceTypes = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "computer_system_pci_device", joinColumns = @JoinColumn(name = "computer_system_id"))
    @OrderColumn(name = "pci_device_order")
    private List<ComputerSystemDevice> pciDevices = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "computer_system_trusted_module", joinColumns = @JoinColumn(name = "computer_system_id"))
    @OrderColumn(name = "trusted_module_order")
    private List<TrustedModule> trustedModules = new ArrayList<>();

    @SuppressEvents
    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Memory> memoryModules = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Processor> processors = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<SimpleStorage> simpleStorages = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Storage> storages = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetInterface> ethernetInterfaces = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PcieDevice> pcieDevices = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<PcieDeviceFunction> pcieDeviceFunctions = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<NetworkInterface> networkInterfaces = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "computer_system_manager",
        joinColumns = {@JoinColumn(name = "computer_system_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @ManyToMany(mappedBy = "computerSystems", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> chassis = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = EAGER, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "computer_system_metadata_id")
    private ComputerSystemMetadata metadata = new ComputerSystemMetadata();

    @OneToOne(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private ComposedNode composedNode;

    @OneToOne(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private ComputerSystemMetrics computerSystemMetrics;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
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

    public String getSerialNumber() {
        return serialNumber;
    }

    public void setSerialNumber(String serialNumber) {
        this.serialNumber = serialNumber;
    }

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public SystemType getSystemType() {
        return systemType;
    }

    public void setSystemType(SystemType systemType) {
        this.systemType = systemType;
    }

    public String getAssetTag() {
        return assetTag;
    }

    public void setAssetTag(String assetTag) {
        this.assetTag = assetTag;
    }

    public String getBiosVersion() {
        return biosVersion;
    }

    public void setBiosVersion(String biosVersion) {
        this.biosVersion = biosVersion;
    }

    public String getSku() {
        return sku;
    }

    public void setSku(String sku) {
        this.sku = sku;
    }

    public String getHostName() {
        return hostName;
    }

    public void setHostName(String hostName) {
        this.hostName = hostName;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public Integer getMemorySockets() {
        return memorySockets;
    }

    public void setMemorySockets(Integer memorySockets) {
        this.memorySockets = memorySockets;
    }

    public Integer getProcessorSockets() {
        return processorSockets;
    }

    public void setProcessorSockets(Integer processorSockets) {
        this.processorSockets = processorSockets;
    }

    public DiscoveryState getDiscoveryState() {
        return discoveryState;
    }

    public void setDiscoveryState(DiscoveryState discoveryState) {
        this.discoveryState = discoveryState;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public String getPartNumber() {
        return partNumber;
    }

    public void setPartNumber(String partNumber) {
        this.partNumber = partNumber;
    }

    public Boolean getUserModeEnabled() {
        return userModeEnabled;
    }

    public void setUserModeEnabled(Boolean userModeEnabled) {
        this.userModeEnabled = userModeEnabled;
    }

    public Boolean getTrustedExecutionTechnologyEnabled() {
        return trustedExecutionTechnologyEnabled;
    }

    public void setTrustedExecutionTechnologyEnabled(Boolean trustedExecutionTechnologyEnabled) {
        this.trustedExecutionTechnologyEnabled = trustedExecutionTechnologyEnabled;
    }

    public Boot getBoot() {
        return boot;
    }

    public void setBoot(Boot boot) {
        this.boot = boot;
    }

    public ProcessorSummary getProcessorSummary() {
        return processorSummary;
    }

    public void setProcessorSummary(ProcessorSummary processorSummary) {
        this.processorSummary = processorSummary;
    }

    public Integer getProcessorsCount() {
        return getProcessorSummary().getCount();
    }

    public String getProcessorsModel() {
        return getProcessorSummary().getModel();
    }

    public Status getProcessorsStatus() {
        return getProcessorSummary().getStatus();
    }

    public MemorySummary getMemorySummary() {
        return memorySummary;
    }

    public void setMemorySummary(MemorySummary memorySummary) {
        this.memorySummary = memorySummary;
    }

    public Status getMemoryStatus() {
        return getMemorySummary().getStatus();
    }

    public BigDecimal getTotalSystemMemoryGiB() {
        return getMemorySummary().getTotalSystemMemoryGiB();
    }

    public List<String> getPcieConnectionIds() {
        return pcieConnectionIds;
    }

    public void addPcieConnectionId(String pcieConnectionId) {
        pcieConnectionIds.add(pcieConnectionId);
    }

    @Override
    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public List<InterfaceType> getAllowableInterfaceTypes() {
        return allowableInterfaceTypes;
    }

    public void addAllowableResetType(ResetType allowableResetType) {
        allowableResetTypes.add(allowableResetType);
    }

    public void addAllowableInterfaceType(InterfaceType interfaceType) {
        allowableInterfaceTypes.add(interfaceType);
    }

    public List<ComputerSystemDevice> getPciDevices() {
        return pciDevices;
    }

    public void addPciDevice(ComputerSystemDevice pciDevice) {
        pciDevices.add(pciDevice);
    }

    public List<TrustedModule> getTrustedModules() {
        return trustedModules;
    }

    public void addTrustedModule(TrustedModule trustedModule) {
        trustedModules.add(trustedModule);
    }

    public Set<Memory> getMemoryModules() {
        return memoryModules;
    }

    public void addMemoryModule(Memory memoryModule) {
        requiresNonNull(memoryModule, "memoryModule");

        memoryModules.add(memoryModule);
        if (!this.equals(memoryModule.getComputerSystem())) {
            memoryModule.setComputerSystem(this);
        }
    }

    public void unlinkMemoryModule(Memory memoryModule) {
        if (memoryModules.contains(memoryModule)) {
            memoryModules.remove(memoryModule);
            if (memoryModule != null) {
                memoryModule.unlinkComputerSystem(this);
            }
        }
    }

    public Set<Processor> getProcessors() {
        return processors;
    }

    public void addProcessor(Processor processor) {
        requiresNonNull(processor, "processor");

        processors.add(processor);
        if (!this.equals(processor.getComputerSystem())) {
            processor.setComputerSystem(this);
        }
    }

    public void unlinkProcessor(Processor processor) {
        if (processors.contains(processor)) {
            processors.remove(processor);
            if (processor != null) {
                processor.unlinkComputerSystem(this);
            }
        }
    }

    public Set<SimpleStorage> getSimpleStorages() {
        return simpleStorages;
    }

    public void addSimpleStorage(SimpleStorage simpleStorage) {
        requiresNonNull(simpleStorage, "simpleStorage");

        simpleStorages.add(simpleStorage);
        if (!this.equals(simpleStorage.getComputerSystem())) {
            simpleStorage.setComputerSystem(this);
        }
    }

    public void unlinkSimpleStorage(SimpleStorage simpleStorage) {
        if (simpleStorages.contains(simpleStorage)) {
            simpleStorages.remove(simpleStorage);
            if (simpleStorage != null) {
                simpleStorage.unlinkComputerSystem(this);
            }
        }
    }

    public Set<Storage> getStorages() {
        return storages;
    }

    public void addStorage(Storage storage) {
        requiresNonNull(storage, "storage");

        storages.add(storage);
        if (!this.equals(storage.getComputerSystem())) {
            storage.setComputerSystem(this);
            linkChassisToStoragesContainedByThisSystem(storage);
        }
    }

    public void unlinkStorage(Storage storage) {
        if (storages.contains(storage)) {
            storages.remove(storage);
            if (storage != null) {
                storage.unlinkComputerSystem(this);

                Iterator<Chassis> chassisIterator = getChassis().iterator();
                if (chassisIterator.hasNext()) {
                    storage.unlinkChassis(chassisIterator.next());
                }
            }
        }
    }

    @Override
    public Set<EthernetInterface> getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public void addEthernetInterface(EthernetInterface ethernetInterface) {
        requiresNonNull(ethernetInterface, "ethernetInterface");

        ethernetInterfaces.add(ethernetInterface);
        if (!this.equals(ethernetInterface.getComputerSystem())) {
            ethernetInterface.setComputerSystem(this);
        }
    }

    public void unlinkEthernetInterface(EthernetInterface ethernetInterface) {
        if (ethernetInterfaces.contains(ethernetInterface)) {
            ethernetInterfaces.remove(ethernetInterface);
            if (ethernetInterface != null) {
                ethernetInterface.unlinkComputerSystem(this);
            }
        }
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!this.equals(endpoint.getComputerSystem())) {
            endpoint.setComputerSystem(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkComputerSystem(this);
            }
        }
    }

    public Set<PcieDevice> getPcieDevices() {
        return pcieDevices;
    }

    public void addPcieDevice(PcieDevice pcieDevice) {
        requiresNonNull(pcieDevice, "pcieDevice");

        pcieDevices.add(pcieDevice);
        if (!this.equals(pcieDevice.getComputerSystem())) {
            pcieDevice.setComputerSystem(this);
        }
    }

    public void unlinkPcieDevice(PcieDevice pcieDevice) {
        if (pcieDevices.contains(pcieDevice)) {
            pcieDevices.remove(pcieDevice);
            if (pcieDevice != null) {
                pcieDevice.unlinkComputerSystem(this);
            }
        }
    }

    public Set<PcieDeviceFunction> getPcieDeviceFunctions() {
        return pcieDeviceFunctions;
    }

    public void addPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        requiresNonNull(pcieDeviceFunction, "pcieDeviceFunction");

        pcieDeviceFunctions.add(pcieDeviceFunction);
        if (!this.equals(pcieDeviceFunction.getComputerSystem())) {
            pcieDeviceFunction.setComputerSystem(this);
        }
    }

    public void unlinkPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        if (pcieDeviceFunctions.contains(pcieDeviceFunction)) {
            pcieDeviceFunctions.remove(pcieDeviceFunction);
            if (pcieDeviceFunction != null) {
                pcieDeviceFunction.unlinkComputerSystem(this);
            }
        }
    }

    public Set<NetworkInterface> getNetworkInterfaces() {
        return networkInterfaces;
    }

    public void addNetworkInterface(NetworkInterface networkInterface) {
        requiresNonNull(networkInterface, "networkInterface");

        networkInterfaces.add(networkInterface);
        if (!this.equals(networkInterface.getComputerSystem())) {
            networkInterface.setComputerSystem(this);
        }
    }

    public void unlinkNetworkInterface(NetworkInterface networkInterface) {
        if (networkInterfaces.contains(networkInterface)) {
            networkInterfaces.remove(networkInterface);
            if (networkInterface != null) {
                networkInterface.unlinkComputerSystem(this);
            }
        }
    }

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getComputerSystems().contains(this)) {
            manager.addComputerSystem(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkComputerSystem(this);
            }
        }
    }

    public Set<Chassis> getChassis() {
        return chassis;
    }

    public void addChassis(Chassis newChassis) {
        requiresNonNull(newChassis, "newChassis");

        chassis.add(newChassis);
        if (!newChassis.getComputerSystems().contains(this)) {
            newChassis.addComputerSystem(this);
        }

        linkToStorages(newChassis);
    }

    public void unlinkChassis(Chassis chassisToUnlink) {
        if (chassis.contains(chassisToUnlink)) {
            chassis.remove(chassisToUnlink);
            uncoupleFromStorages(chassisToUnlink);
            if (chassisToUnlink != null) {
                chassisToUnlink.unlinkComputerSystem(this);
            }
        }
    }

    public ComputerSystemMetadata getMetadata() {
        return metadata;
    }

    @EventRedirectionTarget
    public ComposedNode getComposedNode() {
        return composedNode;
    }

    public void setComposedNode(ComposedNode composedNode) {
        if (!Objects.equals(this.composedNode, composedNode)) {
            unlinkComposedNode(this.composedNode);
            this.composedNode = composedNode;
            if (composedNode != null && !this.equals(composedNode.getComputerSystem())) {
                composedNode.setComputerSystem(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkComputerSystem(this);
            }
        }
    }

    public ComputerSystemMetrics getComputerSystemMetrics() {
        return computerSystemMetrics;
    }

    public void setComputerSystemMetrics(ComputerSystemMetrics computerSystemMetrics) {
        if (!Objects.equals(this.computerSystemMetrics, computerSystemMetrics)) {
            unlinkComputerSystemMetrics(this.computerSystemMetrics);
            this.computerSystemMetrics = computerSystemMetrics;
            if (computerSystemMetrics != null && !this.equals(computerSystemMetrics.getComputerSystem())) {
                computerSystemMetrics.setComputerSystem(this);
            }
        }
    }

    public void unlinkComputerSystemMetrics(ComputerSystemMetrics computerSystemMetrics) {
        if (Objects.equals(this.computerSystemMetrics, computerSystemMetrics)) {
            this.computerSystemMetrics = null;
            if (computerSystemMetrics != null) {
                computerSystemMetrics.unlinkComputerSystem(this);
            }
        }
    }

    public MemoryModule getMemoryModuleFromMemorySummary() {
        return new MemoryModuleFromMemorySummary(this);
    }

    public Optional<Chassis> getDrawerChassis() {
        Set<Chassis> chassisList = getChassis();
        Set<Chassis> allChassisSet = new HashSet<>(chassisList);

        chassisList.stream()
            .map(Chassis::getAllOnTopOfChassis)
            .forEach(allChassisSet::addAll);

        return allChassisSet.stream()
            .filter(chassis -> DRAWER.equals(chassis.getChassisType()))
            .findFirst();
    }

    @Override
    public void preRemove() {
        unlinkCollection(memoryModules, this::unlinkMemoryModule);
        unlinkCollection(processors, this::unlinkProcessor);
        unlinkCollection(simpleStorages, this::unlinkSimpleStorage);
        unlinkCollection(storages, this::unlinkStorage);
        unlinkCollection(ethernetInterfaces, this::unlinkEthernetInterface);
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(pcieDevices, this::unlinkPcieDevice);
        unlinkCollection(pcieDeviceFunctions, this::unlinkPcieDeviceFunction);
        unlinkCollection(networkInterfaces, this::unlinkNetworkInterface);
        unlinkCollection(managers, this::unlinkManager);
        unlinkCollection(chassis, this::unlinkChassis);
        unlinkComposedNode(composedNode);
        unlinkComputerSystemMetrics(computerSystemMetrics);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, chassis);
    }

    @Override
    public String getMultiSourceDiscriminator() {
        return uuid.toString();
    }

    @Override
    public String getResetName() {
        return ENTITY_NAME;
    }

    private void linkToStorages(Chassis newChassis) {
        getStorages().forEach(storage -> storage.setChassis(newChassis));
        getStorages().forEach(storage -> storage.getDrives().forEach(drive -> drive.setChassis(newChassis)));
    }

    private void uncoupleFromStorages(Chassis chassisToUnlink) {
        getStorages().forEach(storage -> storage.unlinkChassis(chassisToUnlink));
        getStorages().forEach(storage -> storage.getDrives().forEach(drive -> drive.unlinkChassis(chassisToUnlink)));
    }

    private void linkChassisToStoragesContainedByThisSystem(Storage storage) {
        Iterator<Chassis> iterator = this.chassis.iterator();
        if (iterator.hasNext()) {
            Chassis chassis = iterator.next();
            storage.setChassis(chassis);
            storage.getDrives().forEach(drive -> drive.setChassis(chassis));
        }
    }
}
