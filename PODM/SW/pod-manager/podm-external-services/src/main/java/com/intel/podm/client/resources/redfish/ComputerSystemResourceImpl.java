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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.LinkName;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.redfish.BootObject;
import com.intel.podm.client.api.resources.redfish.ComputerSystemResource;
import com.intel.podm.client.api.resources.redfish.DeviceObject;
import com.intel.podm.client.api.resources.redfish.LuiComputerSystemResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.actions.ResetType;

import java.util.List;
import java.util.UUID;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.DiscoveryState.BASIC;

@OdataTypes({
        "#ComputerSystem.1.0.0.ComputerSystem",
        "#ComputerSystem.1.0.1.ComputerSystem"
})
public class ComputerSystemResourceImpl extends ExternalServiceResourceImpl implements ComputerSystemResource, LuiComputerSystemResource {
    @JsonProperty("UUID")
    private UUID uuid;
    @JsonProperty("Manufacturer")
    private String manufacturer;
    @JsonProperty("Model")
    private String model;
    @JsonProperty("SerialNumber")
    private String serialNumber;
    @JsonProperty("SystemType")
    private SystemType systemType;
    @JsonProperty("AssetTag")
    private String assetTag;
    @JsonProperty("BiosVersion")
    private String biosVersion;
    @JsonProperty("SKU")
    private String sku;
    @JsonProperty("HostName")
    private String hostName;
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("PowerState")
    private PowerState powerState;
    @JsonProperty("PartNumber")
    private String partNumber;
    @JsonProperty("Memory")
    private ODataId memory;
    @JsonProperty("MemoryChunks")
    private ODataId memoryChunks;
    @JsonProperty("ProcessorSummary")
    private ProcessorSummary processorSummary = new ProcessorSummary();
    @JsonProperty("MemorySummary")
    private MemorySummary memorySummary = new MemorySummary();
    @JsonProperty("Processors")
    private ODataId processors;
    @JsonProperty("EthernetInterfaces")
    private ODataId ethernetInterfaces;
    @JsonProperty("SimpleStorage")
    private ODataId simpleStorages;
    @JsonProperty("Boot")
    private BootObjectImpl boot = new BootObjectImpl();
    @JsonProperty("Oem")
    private Oem oem = new Oem();
    @JsonProperty("Actions")
    private Actions actions = new Actions();
    @JsonProperty("Links")
    private Links links = new Links();

    @Override
    public UUID getUuid() {
        return uuid;
    }

    @Override
    public String getManufacturer() {
        return manufacturer;
    }

    @Override
    public String getModel() {
        return model;
    }

    @Override
    public String getSerialNumber() {
        return serialNumber;
    }

    @Override
    public SystemType getSystemType() {
        return systemType;
    }

    @Override
    public String getAssetTag() {
        return assetTag;
    }

    @Override
    public String getBiosVersion() {
        return biosVersion;
    }

    @Override
    public String getSku() {
        return sku;
    }

    @Override
    public String getHostName() {
        return hostName;
    }

    @Override
    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public PowerState getPowerState() {
        return powerState;
    }

    @Override
    public String getPartNumber() {
        return partNumber;
    }

    @Override
    public BootObject getBoot() {
        return boot;
    }

    @Override
    public Integer getMemorySockets() {
        return oem.rackScaleOem.memorySockets;
    }

    @Override
    public Status getMemoryStatus() {
        return memorySummary.status;
    }

    @Override
    public Integer getTotalSystemMemoryGiB() {
        return memorySummary.totalSystemMemoryGiB;
    }

    @Override
    public Integer getProcessorsCount() {
        return processorSummary.count;
    }

    @Override
    public String getProcessorsModel() {
        return processorSummary.model;
    }

    @Override
    public Integer getProcessorsSockets() {
        return oem.rackScaleOem.processorSockets;
    }

    @Override
    public Status getProcessorsStatus() {
        return processorSummary.status;
    }

    @Override
    public boolean isBasic() {
        return BASIC.equals(oem.rackScaleOem.discoveryState);
    }

    @Override
    public List<ResetType> getAllowableResetTypes() {
        return actions.reset.allowableValues;
    }

    @Override
    public Iterable<DeviceObject> getPciDevices() {
        return (Iterable) oem.rackScaleOem.pciDevices;
    }

    @Override
    @LinkName("processors")
    public Iterable<ResourceSupplier> getProcessors() throws ExternalServiceApiReaderException {
        return processMembersListResource(processors);
    }

    @Override
    @LinkName("ethernetInterfaces")
    public Iterable<ResourceSupplier> getEthernetInterfaces() throws ExternalServiceApiReaderException {
        return processMembersListResource(ethernetInterfaces);
    }

    @Override
    @LinkName("memoryChunks")
    public Iterable<ResourceSupplier> getMemoryChunks() throws ExternalServiceApiReaderException {
        return processMembersListResource(memoryChunks);
    }

    @Override
    @LinkName("memoryModules")
    public Iterable<ResourceSupplier> getMemoryModules() throws ExternalServiceApiReaderException {
        return processMembersListResource(memory);
    }

    @Override
    @LinkName("simpleStorages")
    public Iterable<ResourceSupplier> getSimpleStorages() throws ExternalServiceApiReaderException {
        return processMembersListResource(simpleStorages);
    }

    @Override
    @LinkName("adapters")
    public Iterable<ResourceSupplier> getAdapters() throws ExternalServiceApiReaderException {
        return processMembersListResource(oem.rackScaleOem.adapters);
    }

    private static class ProcessorSummary {
        @JsonProperty("Count")
        private Integer count;
        @JsonProperty("Model")
        private String model;
        @JsonProperty("Status")
        private Status status;
    }

    private static class MemorySummary {
        @JsonProperty("TotalSystemMemoryGiB")
        private Integer totalSystemMemoryGiB;
        @JsonProperty("Status")
        private Status status;
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonIgnoreProperties(ignoreUnknown = true)
        private static class RackScaleOem {
            @JsonProperty("DiscoveryState")
            private DiscoveryState discoveryState;
            @JsonProperty("ProcessorSockets")
            private Integer processorSockets;
            @JsonProperty("MemorySockets")
            private Integer memorySockets;
            @JsonProperty("PciDevices")
            private Iterable<DeviceObjectImpl> pciDevices = newArrayList();
            @JsonProperty("Adapters")
            private ODataId adapters;
        }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static class Actions {
        @JsonProperty("#ComputerSystem.Reset")
        private Reset reset = new Reset();

        @JsonIgnoreProperties(ignoreUnknown = true)
        private static class Reset {
            @JsonProperty("ResetType@Redfish.AllowableValues")
            private List<ResetType> allowableValues;
        }
    }

    @JsonIgnoreProperties(ignoreUnknown = true)
    private static class Links {
    }
}
