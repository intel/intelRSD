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

import com.intel.rsd.nodecomposer.persistence.IgnoreUnlinkingRelationship;
import com.intel.rsd.nodecomposer.persistence.listeners.ComputerSystemListener;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.MemoryModule;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Boot;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.MemorySummary;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.TrustedModule;
import com.intel.rsd.nodecomposer.types.PerformanceConfigurationType;
import com.intel.rsd.nodecomposer.types.PowerState;
import com.intel.rsd.nodecomposer.types.SystemType;
import com.intel.rsd.nodecomposer.types.actions.ResetType;
import lombok.Getter;
import lombok.Setter;
import lombok.val;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
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
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

import static com.intel.rsd.collections.IterableHelper.optionalSingle;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.util.stream.Collectors.toList;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.CascadeType.REMOVE;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "computer_system")
@NamedQueries({
    @NamedQuery(name = ComputerSystem.GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE, query =
        "SELECT computerSystem FROM ComputerSystem computerSystem "
            + "WHERE computerSystem.metadata.allocated = :allocated "
            + "AND computerSystem.systemType = :systemType"),
    @NamedQuery(name = ComputerSystem.GET_PHYSICAL_COMPUTER_SYSTEM_BY_UUID, query =
        "SELECT computerSystem FROM ComputerSystem computerSystem "
            + "WHERE computerSystem.uuid = :uuid "
            + "AND computerSystem.systemType = :systemType")
})
@EntityListeners(ComputerSystemListener.class)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class ComputerSystem extends DiscoverableEntity implements ComposableAsset {
    public static final String GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE = "GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE";
    public static final String GET_PHYSICAL_COMPUTER_SYSTEM_BY_UUID = "GET_PHYSICAL_COMPUTER_SYSTEM_BY_UUID";
    private static final String ENTITY_NAME = "ComputerSystem";

    @Getter
    @Setter
    @Column(name = "uuid")
    private UUID uuid;

    @Getter
    @Setter
    @Column(name = "system_type")
    @Enumerated(STRING)
    private SystemType systemType;

    @Getter
    @Setter
    @Column(name = "asset_tag")
    private String assetTag;

    @Getter
    @Setter
    @Column(name = "power_state")
    @Enumerated(STRING)
    private PowerState powerState;

    @Getter
    @Setter
    @Column(name = "user_mode_enabled")
    private Boolean userModeEnabled;

    @Getter
    @Setter
    @Column(name = "trusted_execution_technology_enabled")
    private Boolean trustedExecutionTechnologyEnabled;

    @Column(name = "achievable")
    private Boolean achievable = true;

    @Embedded
    private Boot boot;

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
    @CollectionTable(name = "computer_system_performance_configuration_type", joinColumns = @JoinColumn(name = "computer_system_id"))
    @Column(name = "performance_configuration_type")
    @OrderColumn(name = "performance_configuration_type_order")
    private List<PerformanceConfigurationType> configurationTypes = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "computer_system_trusted_module", joinColumns = @JoinColumn(name = "computer_system_id"))
    @OrderColumn(name = "trusted_module_order")
    private List<TrustedModule> trustedModules = new ArrayList<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Memory> memoryModules = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Processor> processors = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<SimpleStorage> simpleStorages = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Storage> storages = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<StorageService> storageServices = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetInterface> ethernetInterfaces = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @OneToMany(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<NetworkInterface> networkInterfaces = new HashSet<>();

    @ManyToMany(mappedBy = "computerSystems", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Chassis> chassis = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "computer_system_manager",
        joinColumns = {@JoinColumn(name = "computer_system_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = EAGER, cascade = {MERGE, PERSIST, REMOVE})
    @JoinColumn(name = "computer_system_metadata_id")
    private ComputerSystemMetadata metadata = new ComputerSystemMetadata();

    @OneToOne(mappedBy = "computerSystem", fetch = LAZY, cascade = {MERGE, PERSIST})
    private ComposedNode composedNode;

    public Boot getBoot() {
        return boot;
    }

    public void setBoot(Boot boot) {
        this.boot = boot;
    }

    public MemorySummary getMemorySummary() {
        return memorySummary;
    }

    public void setMemorySummary(MemorySummary memorySummary) {
        this.memorySummary = memorySummary;
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

    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public void addAllowableResetType(ResetType allowableResetType) {
        allowableResetTypes.add(allowableResetType);
    }

    public List<PerformanceConfigurationType> getConfigurationTypes() {
        return configurationTypes;
    }

    public void addConfigurationType(PerformanceConfigurationType configurationType) {
        this.configurationTypes.add(configurationType);
    }

    public List<TrustedModule> getTrustedModules() {
        return trustedModules;
    }

    public void addTrustedModule(TrustedModule trustedModule) {
        trustedModules.add(trustedModule);
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

    public Set<StorageService> getStorageServices() {
        return storageServices;
    }

    public void addStorageService(StorageService storageService) {
        requiresNonNull(storageService, "storageService");
        storageServices.add(storageService);
        if (!this.equals(storageService.getComputerSystem())) {
            storageService.setComputerSystem(this);
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (storageServices.contains(storageService)) {
            storageServices.remove(storageService);
            if (storageService != null) {
                storageService.unlinkComputerSystem(this);
            }
        }
    }

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

    public MemoryModule getMemoryModuleFromMemorySummary() {
        return new MemoryModuleFromMemorySummary(this);
    }

    public Optional<NetworkDeviceFunction> getNetworkDeviceFunction() {
        val networkDeviceFunctionFromNetworkAdapters = getNetworkDeviceFunctionFromNetworkAdapters();

        if (networkDeviceFunctionFromNetworkAdapters.isPresent()) {
            return networkDeviceFunctionFromNetworkAdapters;
        } else {
            return getNetworkDeviceFunctionFromNetworkInterfaces();
        }
    }

    private Optional<NetworkDeviceFunction> getNetworkDeviceFunctionFromNetworkInterfaces() {
        return optionalSingle(networkInterfaces.stream()
            .map(NetworkInterface::getNetworkDeviceFunctions)
            .flatMap(Collection::stream)
            .collect(toList()));
    }

    private Optional<NetworkDeviceFunction> getNetworkDeviceFunctionFromNetworkAdapters() {
        val chassis = optionalSingle(getChassis());

        if (chassis.isPresent()) {
            val networkAdapters = chassis.get().getNetworkAdapters();

            return optionalSingle(networkAdapters.stream()
                .map(NetworkAdapter::getNetworkDeviceFunctions)
                .flatMap(Collection::stream)
                .collect(toList()));
        }

        return Optional.empty();
    }

    @Override
    public void preRemove() {
        unlinkCollection(memoryModules, this::unlinkMemoryModule);
        unlinkCollection(processors, this::unlinkProcessor);
        unlinkCollection(simpleStorages, this::unlinkSimpleStorage);
        unlinkCollection(storages, this::unlinkStorage);
        unlinkCollection(storageServices, this::unlinkStorageService);
        unlinkCollection(ethernetInterfaces, this::unlinkEthernetInterface);
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(networkInterfaces, this::unlinkNetworkInterface);
        unlinkCollection(chassis, this::unlinkChassis);
        unlinkCollection(managers, this::unlinkManager);
        unlinkComposedNode(composedNode);
    }

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

    public boolean hasNetworkInterfaceWithNetworkDeviceFunction() {
        return getNetworkDeviceFunctionFromNetworkAdapters().isPresent()
            || getNetworkDeviceFunctionFromNetworkInterfaces().isPresent();
    }

    @Override
    public boolean isAchievable() {
        return achievable;
    }

    @Override
    public void setAchievable(boolean achievable) {
        this.achievable = achievable;
    }
}
