/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperties;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.DeepDiscoverable;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.Manageable;
import com.intel.podm.business.entities.redfish.base.MemoryModule;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.base.StorageServiceHost;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.business.entities.redfish.properties.Boot;
import com.intel.podm.business.entities.redfish.properties.ComputerSystemDevice;
import com.intel.podm.common.types.MemoryDeviceType;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.helpers.EnumeratedTypeListHolder;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.UUID;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.business.entities.base.DomainObjectLink.CHASSIS;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.INCLUDED;
import static com.intel.podm.business.entities.base.DomainObjectLink.MANAGED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.PCI_DEVICES;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumeratedTypeListProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.statusProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.RUNNING;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.WAITING_TO_START;
import static com.intel.podm.common.types.ChassisType.DRAWER;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.enumeratedTypeListHolder;
import static com.intel.podm.common.types.helpers.EnumeratedTypeListHolder.toList;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.lang.Boolean.TRUE;
import static java.util.Arrays.stream;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class ComputerSystem extends DomainObject implements DeepDiscoverable, Discoverable, Manageable, StatusPossessor, StorageServiceHost, Descriptable {
    public static final DomainObjectProperty<String> MANUFACTURER = stringProperty("manufacturer");
    public static final DomainObjectProperty<String> MODEL = stringProperty("model");
    public static final DomainObjectProperty<String> SERIAL_NUMBER = stringProperty("serialNumber");
    public static final DomainObjectProperty<UUID> UUID = DomainObjectProperties.uuidProperty("uuid");
    public static final DomainObjectProperty<SystemType> SYSTEM_TYPE = enumProperty("systemType", SystemType.class);
    public static final DomainObjectProperty<String> ASSET_TAG = stringProperty("assetTag");
    public static final DomainObjectProperty<String> BIOS_VERSION = stringProperty("biosVersion");
    public static final DomainObjectProperty<String> SKU = stringProperty("sku");
    public static final DomainObjectProperty<String> HOST_NAME = stringProperty("hostName");
    public static final DomainObjectProperty<IndicatorLed> INDICATOR_LED = enumProperty("indicatorLed", IndicatorLed.class);
    public static final DomainObjectProperty<Integer> MEMORY_SOCKETS = integerProperty("memorySockets");
    public static final DomainObjectProperty<Status> MEMORY_STATUS = statusProperty("memoryStatus");
    public static final DomainObjectProperty<Integer> TOTAL_SYSTEM_MEMORY_GIB = integerProperty("totalSystemMemoryGiB");
    public static final DomainObjectProperty<Integer> PROCESSORS_COUNT = integerProperty("processorsCount");
    public static final DomainObjectProperty<String> PROCESSORS_MODEL = stringProperty("processorsModel");
    public static final DomainObjectProperty<Integer> PROCESSORS_SOCKETS = integerProperty("processorsSockets");
    public static final DomainObjectProperty<Status> PROCESSORS_STATUS = statusProperty("processorsStatus");
    public static final DomainObjectProperty<DiscoveryState> DISCOVERY_STATE = enumProperty("discoveryState", DiscoveryState.class);
    public static final DomainObjectProperty<PowerState> POWER_STATE = enumProperty("powerState", PowerState.class);
    public static final DomainObjectProperty<String> PART_NUMBER = stringProperty("partNumber");
    public static final DomainObjectProperty<EnumeratedTypeListHolder<ResetType>> ALLOWABLE_RESET_TYPES =
            enumeratedTypeListProperty("allowableResetTypes", ResetType.class);

    public static final DomainObjectProperty<Boolean> ALLOCATED = DomainObjectProperties.booleanProperty("allocated");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }

    public String getManufacturer() {
        return getProperty(MANUFACTURER);
    }

    public void setManufacturer(String manufacturer) {
        setProperty(MANUFACTURER, manufacturer);
    }

    public String getModel() {
        return getProperty(MODEL);
    }

    public void setModel(String model) {
        setProperty(MODEL, model);
    }

    public String getSerialNumber() {
        return getProperty(SERIAL_NUMBER);
    }

    public void setSerialNumber(String serialNumber) {
        setProperty(SERIAL_NUMBER, serialNumber);
    }

    public void setUuid(UUID uuid) {
        setProperty(UUID, uuid);
    }

    public UUID getUuid() {
        return getProperty(UUID);
    }

    public SystemType getSystemType() {
        return getProperty(SYSTEM_TYPE);
    }

    public void setSystemType(SystemType systemType) {
        setProperty(SYSTEM_TYPE, systemType);
    }

    public String getAssetTag() {
        return getProperty(ASSET_TAG);
    }

    public void setAssetTag(String assetTag) {
        setProperty(ASSET_TAG, assetTag);
    }

    public String getBiosVersion() {
        return getProperty(BIOS_VERSION);
    }

    public void setBiosVersion(String biosVersion) {
        setProperty(BIOS_VERSION, biosVersion);
    }

    public String getSku() {
        return getProperty(SKU);
    }

    public void setSku(String sku) {
        setProperty(SKU, sku);
    }

    public String getHostName() {
        return getProperty(HOST_NAME);
    }

    public void setHostName(String hostName) {
        setProperty(HOST_NAME, hostName);
    }

    public IndicatorLed getIndicatorLed() {
        return getProperty(INDICATOR_LED);
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        setProperty(INDICATOR_LED, indicatorLed);
    }

    public Integer getMemorySockets() {
        return getProperty(MEMORY_SOCKETS);
    }

    public void setMemorySockets(Integer memorySockets) {
        setProperty(MEMORY_SOCKETS, memorySockets);
    }

    public Status getMemoryStatus() {
        return getProperty(MEMORY_STATUS);
    }

    public void setMemoryStatus(Status memoryStatus) {
        setProperty(MEMORY_STATUS, memoryStatus);
    }

    public Integer getTotalSystemMemoryGiB() {
        return getProperty(TOTAL_SYSTEM_MEMORY_GIB);
    }

    public void setTotalSystemMemoryGiB(Integer totalSystemMemoryGiB) {
        setProperty(TOTAL_SYSTEM_MEMORY_GIB, totalSystemMemoryGiB);
    }

    public MemoryModule getMemoryModuleFromMemorySummary() {
        return new MemoryModuleFromMemorySummary(this);
    }

    public Integer getProcessorsCount() {
        return getProperty(PROCESSORS_COUNT);
    }

    public void setProcessorsCount(Integer processorsCount) {
        setProperty(PROCESSORS_COUNT, processorsCount);
    }

    public String getProcessorsModel() {
        return getProperty(PROCESSORS_MODEL);
    }

    public void setProcessorsModel(String processorsModel) {
        setProperty(PROCESSORS_MODEL, processorsModel);
    }

    public Integer getProcessorsSockets() {
        return getProperty(PROCESSORS_SOCKETS);
    }

    public void setProcessorsSockets(Integer processorsSockets) {
        setProperty(PROCESSORS_SOCKETS, processorsSockets);
    }

    public Status getProcessorsStatus() {
        return getProperty(PROCESSORS_STATUS);
    }

    public void setProcessorsStatus(Status processorsStatus) {
        setProperty(PROCESSORS_STATUS, processorsStatus);
    }

    public DiscoveryState getDiscoveryState() {
        return getProperty(DISCOVERY_STATE);
    }

    public void setDiscoveryState(DiscoveryState discoveryState) {
        setProperty(DISCOVERY_STATE, discoveryState);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public PowerState getPowerState() {
        return getProperty(POWER_STATE);
    }

    public void setPowerState(PowerState powerState) {
        setProperty(POWER_STATE, powerState);
    }

    public String getPartNumber() {
        return getProperty(PART_NUMBER);
    }

    public void setPartNumber(String partNumber) {
        setProperty(PART_NUMBER, partNumber);
    }

    public List<ResetType> getAllowableResetTypes() {
        return toList(ResetType.class, getProperty(ALLOWABLE_RESET_TYPES));
    }

    public void setAllowableResetTypes(List<ResetType> allowableResetTypes) {
        setProperty(ALLOWABLE_RESET_TYPES, enumeratedTypeListHolder(ResetType.class, allowableResetTypes));
    }

    public Boolean isAllocated() {
        return Objects.equals(TRUE, getProperty(ALLOCATED));
    }

    public void setAllocated(Boolean allocated) {
        setProperty(ALLOCATED, allocated);
    }

    public Boot getBoot() {
        return singleOrNull(getLinked(CONTAINS, Boot.class));
    }

    public void setBoot(Boot boot) {
        link(CONTAINS, boot);
    }

    public List<Chassis> getChassis() {
        return getLinked(CHASSIS);
    }

    public List<ComputerSystemDevice> getPciDevices() {
        return getLinked(PCI_DEVICES, ComputerSystemDevice.class);
    }

    public ComputerSystemDevice addPciDevice() {
        return addDomainObject(PCI_DEVICES, ComputerSystemDevice.class);
    }

    public List<Adapter> getAdapters() {
        return getLinked(CONTAINS, Adapter.class);
    }

    public void link(Adapter adapter) {
        link(CONTAINS, adapter);
    }

    public void link(MemoryChunk memoryChunk) {
        link(CONTAINS, memoryChunk);
    }

    public List<MemoryChunk> getMemoryChunks() {
        return getLinked(CONTAINS, MemoryChunk.class);
    }

    public List<Memory> getMemoryModules() {
        return getLinked(CONTAINS, Memory.class);
    }

    public void link(Memory memory) {
        link(CONTAINS, memory);
    }

    @Override
    public List<Manager> getManagers() {
        return getLinked(MANAGED_BY);
    }

    @Override
    public void addManager(Manager manager) {
        link(MANAGED_BY, manager);
    }

    public void linkProcessor(Processor processor) {
        link(CONTAINS, processor);
    }

    public void linkEthernetInterface(EthernetInterface ethernetInterface) {
        link(CONTAINS, ethernetInterface);
    }

    public void linkSimpleStorage(SimpleStorage simpleStorage) {
        link(CONTAINS, simpleStorage);
    }

    public List<Processor> getProcessors() {
        return getLinked(CONTAINS, Processor.class);
    }

    public List<SimpleStorage> getSimpleStorages() {
        return getLinked(CONTAINS, SimpleStorage.class);
    }

    public List<EthernetInterface> getEthernetInterfaces() {
        return getLinked(CONTAINS, EthernetInterface.class);
    }

    public ComposedNode getComposedNode() {
        return singleOrNull(getLinked(INCLUDED, ComposedNode.class));
    }

    @Override
    public void setStorageServiceHost(boolean storageServiceHost) {
        setProperty(STORAGE_SERVICE_HOST, storageServiceHost);
    }

    @Override
    public boolean isStorageServiceHost() {
        return Objects.equals(TRUE, getProperty(STORAGE_SERVICE_HOST));
    }

    @Override
    public void setTaskUuid(UUID taskUuid) {
        setProperty(TASK_UUID, taskUuid);
    }

    @Override
    public UUID getTaskUuid() {
        return getProperty(TASK_UUID);
    }

    @Override
    public DeepDiscoveryState getDeepDiscoveryState() {
        return getProperty(DEEP_DISCOVERY_STATE);
    }

    @Override
    public void setDeepDiscoveryState(DeepDiscoveryState deepDiscoveryState) {
        setProperty(DEEP_DISCOVERY_STATE, deepDiscoveryState);
    }

    @Override
    public boolean isInAnyOfStates(DeepDiscoveryState... states) {
        DeepDiscoveryState actualDeepDiscoveryState = getDeepDiscoveryState();
        return actualDeepDiscoveryState != null
                && stream(states).anyMatch(expectedDeepDiscoveryState -> actualDeepDiscoveryState == expectedDeepDiscoveryState);
    }

    @Override
    public boolean isBeingDeepDiscovered() {
        return isInAnyOfStates(WAITING_TO_START, RUNNING);
    }

    public Optional<Chassis> getDrawerChassis() {
        List<Chassis> chassisList = getChassis();
        Set<Chassis> allChassisSet = newHashSet(chassisList);

        for (Chassis chassis : chassisList) {
            allChassisSet.addAll(chassis.getAllOnTopOfChassis());
        }

        return allChassisSet.stream()
                .filter(chassis -> DRAWER.equals(chassis.getChassisType()))
                .findFirst();
    }

    public static class MemoryModuleFromMemorySummary implements MemoryModule {
        public static final int GIB_TO_MIB_RATIO = 1024;
        private Integer capacityMib;

        public MemoryModuleFromMemorySummary(ComputerSystem computerSystem) {
            Integer totalSystemMemoryGiB = computerSystem.getTotalSystemMemoryGiB();
            capacityMib = totalSystemMemoryGiB != null ? totalSystemMemoryGiB * GIB_TO_MIB_RATIO : null;
        }

        @Override
        public Id getId() {
            return null;
        }

        @Override
        public MemoryDeviceType getMemoryDeviceType() {
            return null;
        }

        @Override
        public Integer getCapacityMib() {
            return capacityMib;
        }

        @Override
        public Integer getDataWidthBits() {
            return null;
        }

        @Override
        public String getManufacturer() {
            return null;
        }

        @Override
        public Integer getOperatingSpeedMhz() {
            return null;
        }
    }
}
