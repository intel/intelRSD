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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.common.types.annotations.AsUnassigned;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.DiscoveryState.BASIC;
import static com.intel.podm.common.types.Ref.unassigned;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.podm.common.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@OdataTypes({
    "#ComputerSystem" + VERSION_PATTERN + "ComputerSystem"
})
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class ComputerSystemResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("UUID")
    @AsUnassigned(WHEN_NULL)
    private Ref<UUID> uuid = unassigned();
    @JsonProperty("Manufacturer")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> manufacturer = unassigned();
    @JsonProperty("Model")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> model = unassigned();
    @JsonProperty("SerialNumber")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> serialNumber = unassigned();
    @JsonProperty("SystemType")
    @AsUnassigned(WHEN_NULL)
    private Ref<SystemType> systemType = unassigned();
    @JsonProperty("AssetTag")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> assetTag = unassigned();
    @JsonProperty("BiosVersion")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> biosVersion = unassigned();
    @JsonProperty("SKU")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> sku = unassigned();
    @JsonProperty("HostName")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> hostName = unassigned();
    @JsonProperty("IndicatorLED")
    @AsUnassigned(WHEN_NULL)
    private Ref<IndicatorLed> indicatorLed = unassigned();
    @JsonProperty("Status")
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();
    @JsonProperty("PowerState")
    @AsUnassigned(WHEN_NULL)
    private Ref<PowerState> powerState = unassigned();
    @JsonProperty("PartNumber")
    @AsUnassigned(WHEN_NULL)
    private Ref<String> partNumber = unassigned();
    @JsonProperty("Memory")
    private ODataId memory;
    @JsonProperty("ProcessorSummary")
    private ProcessorSummaryObject processorSummary = new ProcessorSummaryObject();
    @JsonProperty("MemorySummary")
    private MemorySummaryObject memorySummary = new MemorySummaryObject();
    @JsonProperty("Processors")
    private ODataId processors;
    @JsonProperty("EthernetInterfaces")
    private ODataId ethernetInterfaces;
    @JsonProperty("SimpleStorage")
    private ODataId simpleStorages;
    @JsonProperty("Storage")
    private ODataId storages;
    @JsonProperty("PCIeDevices")
    private Set<ODataId> pcieDevices = new LinkedHashSet<>();
    @JsonProperty("PCIeFunctions")
    private Set<ODataId> pcieFunctions = new LinkedHashSet<>();
    @JsonProperty("Boot")
    private BootObject boot = new BootObject();
    @JsonProperty("NetworkInterfaces")
    private ODataId networkInterfaces;
    @JsonProperty("TrustedModules")
    @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
    private Ref<List<TrustedModuleObject>> trustedModules = unassigned();
    @JsonProperty("Oem")
    private Oem oem = new Oem();
    @JsonProperty("Actions")
    private Actions actions = new Actions();
    @JsonProperty("Links")
    private Links links = new Links();

    public Ref<UUID> getUuid() {
        return uuid;
    }

    public Ref<String> getManufacturer() {
        return manufacturer;
    }

    public Ref<String> getModel() {
        return model;
    }

    public Ref<String> getSerialNumber() {
        return serialNumber;
    }

    public Ref<SystemType> getSystemType() {
        return systemType;
    }

    public Ref<String> getAssetTag() {
        return assetTag;
    }

    public Ref<String> getBiosVersion() {
        return biosVersion;
    }

    public Ref<String> getSku() {
        return sku;
    }

    public Ref<String> getHostName() {
        return hostName;
    }

    public Ref<IndicatorLed> getIndicatorLed() {
        return indicatorLed;
    }

    public Ref<Status> getStatus() {
        return status;
    }

    public Ref<PowerState> getPowerState() {
        return powerState;
    }

    public Ref<String> getPartNumber() {
        return partNumber;
    }

    public BootObject getBootObject() {
        return boot;
    }

    public ProcessorSummaryObject getProcessorSummary() {
        return processorSummary;
    }

    public MemorySummaryObject getMemorySummary() {
        return memorySummary;
    }

    public Ref<List<String>> getPcieConnectionId() {
        return oem.rackScaleOem.pcieConnectionId;
    }

    public Ref<Integer> getMemorySockets() {
        return oem.rackScaleOem.memorySockets;
    }

    public Ref<Integer> getProcessorSockets() {
        return oem.rackScaleOem.processorSockets;
    }

    public Ref<List<RedfishComputerSystem.TrustedModule>> getTrustedModules() {
        return (Ref) trustedModules;
    }

    public Ref<Boolean> getUserModeEnabled() {
        return oem.rackScaleOem.userModeEnabled;
    }

    public Ref<Boolean> getTrustedExecutionTechnologyEnabled() {
        return oem.rackScaleOem.trustedExecutionTechnologyEnabled;
    }

    public boolean isBasic() {
        return BASIC.equals(oem.rackScaleOem.discoveryState);
    }

    public Ref<LinkedHashSet<ResetType>> getAllowableResetTypes() {
        return actions.reset.allowableValues;
    }

    public Ref<LinkedHashSet<InterfaceType>> getAllowableInterfaceTypes() {
        return actions.oem.changeTpmState.allowableValues;
    }

    public Ref<LinkedHashSet<RedfishComputerSystem.Device>> getPciDevices() {
        return (Ref) oem.rackScaleOem.pciDevices;
    }

    @LinkName("storage")
    public Iterable<ResourceSupplier> getStorages() throws WebClientRequestException {
        return processMembersListResource(storages);
    }

    @LinkName("endpoints")
    public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
        return toSuppliers(links.endpoints);
    }

    @LinkName("processors")
    public Iterable<ResourceSupplier> getProcessors() throws WebClientRequestException {
        return processMembersListResource(processors);
    }

    @LinkName("ethernetInterfaces")
    public Iterable<ResourceSupplier> getEthernetInterfaces() throws WebClientRequestException {
        return processMembersListResource(ethernetInterfaces);
    }

    @LinkName("memoryModules")
    public Iterable<ResourceSupplier> getMemoryModules() throws WebClientRequestException {
        return processMembersListResource(memory);
    }

    @LinkName("simpleStorages")
    public Iterable<ResourceSupplier> getSimpleStorages() throws WebClientRequestException {
        return processMembersListResource(simpleStorages);
    }

    @LinkName("pcieDevices")
    public Iterable<ResourceSupplier> getPcieDevices() throws WebClientRequestException {
        return toSuppliers(pcieDevices);
    }

    @LinkName("pcieFunctions")
    public Iterable<ResourceSupplier> getPcieFunctions() throws WebClientRequestException {
        return toSuppliers(pcieFunctions);
    }

    @LinkName("adapters")
    public Iterable<ResourceSupplier> getAdapters() throws WebClientRequestException {
        return processMembersListResource(oem.rackScaleOem.adapters);
    }

    @LinkName("networkInterfaces")
    public Iterable<ResourceSupplier> getNetworkInterfaces() throws WebClientRequestException {
        return processMembersListResource(networkInterfaces);
    }

    @LinkName("computerSystemMetrics")
    public ResourceSupplier getComputerSystemMetrics() throws WebClientRequestException {
        if (oem.rackScaleOem.computerSystemMetrics == null) {
            return null;
        }

        return toSupplier(oem.rackScaleOem.computerSystemMetrics);
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("DiscoveryState")
            private DiscoveryState discoveryState;
            @JsonProperty("ProcessorSockets")
            @AsUnassigned(WHEN_NULL)
            private Ref<Integer> processorSockets = unassigned();
            @JsonProperty("MemorySockets")
            @AsUnassigned(WHEN_NULL)
            private Ref<Integer> memorySockets = unassigned();
            @JsonProperty("PciDevices")
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<LinkedHashSet<ComputerSystemDeviceObject>> pciDevices = unassigned();
            @JsonProperty("PCIeConnectionId")
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<List<String>> pcieConnectionId = unassigned();
            @JsonProperty("Adapters")
            private ODataId adapters;
            @JsonProperty("UserModeEnabled")
            private Ref<Boolean> userModeEnabled = unassigned();
            @JsonProperty("TrustedExecutionTechnologyEnabled")
            private Ref<Boolean> trustedExecutionTechnologyEnabled = unassigned();
            @JsonProperty("Metrics")
            private ODataId computerSystemMetrics;
        }
    }

    public class Actions extends RedfishActions {
        @JsonProperty("#ComputerSystem.Reset")
        private Reset reset = new Reset();
        @JsonProperty("Oem")
        private Oem oem = new Oem();

        public class Reset {
            @JsonProperty("ResetType@Redfish.AllowableValues")
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<LinkedHashSet<ResetType>> allowableValues = unassigned();
        }

        @OemType(OEM_IN_ACTIONS)
        public class Oem extends RedfishOem {
            @JsonProperty("#Intel.Oem.StartDeepDiscovery")
            private StartDeepDiscovery startDeepDiscovery = new StartDeepDiscovery();

            @JsonProperty("#Intel.Oem.ChangeTPMState")
            private ChangeTpmState changeTpmState = new ChangeTpmState();

            public class StartDeepDiscovery {
            }

            public class ChangeTpmState {
                @JsonProperty("InterfaceType@Redfish.AllowableValues")
                @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
                private Ref<LinkedHashSet<InterfaceType>> allowableValues = unassigned();

                public ChangeTpmState() {
                }
            }
        }
    }

    public class Links extends RedfishLinks {
        @JsonProperty("Endpoints")
        private Set<ODataId> endpoints;
    }
}
