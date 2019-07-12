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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonSetter;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.LinkName;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.reader.ResourceSupplier;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;
import com.intel.rsd.nodecomposer.types.PowerState;
import com.intel.rsd.nodecomposer.types.Ref;
import com.intel.rsd.nodecomposer.types.Status;
import com.intel.rsd.nodecomposer.types.SystemType;
import com.intel.rsd.nodecomposer.types.actions.ResetType;
import com.intel.rsd.nodecomposer.types.annotations.AsUnassigned;

import java.util.ArrayList;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.Nulls.AS_EMPTY;
import static com.intel.rsd.nodecomposer.types.Ref.unassigned;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_EMPTY_COLLECTION;
import static com.intel.rsd.nodecomposer.types.annotations.AsUnassigned.Strategy.WHEN_NULL;
import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#ComputerSystem" + VERSION_PATTERN + "ComputerSystem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class ComputerSystemResource extends ExternalServiceResourceImpl {
    @JsonSetter(value = "UUID", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<UUID> uuid = unassigned();
    @JsonSetter(value = "SystemType", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<SystemType> systemType = unassigned();
    @JsonSetter(value = "AssetTag", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<String> assetTag = unassigned();
    @JsonSetter(value = "PowerState", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<PowerState> powerState = unassigned();
    @JsonSetter(value = "Status", nulls = AS_EMPTY)
    @AsUnassigned(WHEN_NULL)
    private Ref<Status> status = unassigned();
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
    @JsonProperty("Boot")
    private BootObject boot = new BootObject();
    @JsonProperty("NetworkInterfaces")
    private ODataId networkInterfaces;
    @JsonSetter(value = "TrustedModules", nulls = AS_EMPTY)
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

    public Ref<SystemType> getSystemType() {
        return systemType;
    }

    public Ref<String> getAssetTag() {
        return assetTag;
    }

    public Ref<PowerState> getPowerState() {
        return powerState;
    }

    public Ref<Status> getStatus() {
        return status;
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

    public Ref<List<TrustedModuleObject>> getTrustedModules() {
        return trustedModules;
    }

    public Ref<Boolean> getUserModeEnabled() {
        return oem.rackScaleOem.userModeEnabled;
    }

    public Ref<Boolean> getTrustedExecutionTechnologyEnabled() {
        return oem.rackScaleOem.trustedExecutionTechnologyEnabled;
    }

    public Ref<LinkedHashSet<ResetType>> getAllowableResetTypes() {
        return actions.reset.allowableValues;
    }

    public Oem.RackScaleOem.PerformanceConfiguration getPerformanceConfiguration() {
        return oem.rackScaleOem.performanceConfiguration;
    }

    @LinkName("storage")
    public Iterable<ResourceSupplier> getStorages() throws WebClientRequestException {
        return processMembersListResource(storages);
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

    @LinkName("adapters")
    public Iterable<ResourceSupplier> getAdapters() throws WebClientRequestException {
        return processMembersListResource(oem.rackScaleOem.adapters);
    }

    @LinkName("networkInterfaces")
    public Iterable<ResourceSupplier> getNetworkInterfaces() throws WebClientRequestException {
        return processMembersListResource(networkInterfaces);
    }

    @LinkName("endpoints")
    public Iterable<ResourceSupplier> getEndpoints() throws WebClientRequestException {
        return toSuppliers(links.endpoints);
    }

    public class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonSetter(value = "PCIeConnectionId", nulls = AS_EMPTY)
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<List<String>> pcieConnectionId = unassigned();
            @JsonProperty("Adapters")
            private ODataId adapters;
            @JsonSetter(value = "UserModeEnabled", nulls = AS_EMPTY)
            private Ref<Boolean> userModeEnabled = unassigned();
            @JsonSetter(value = "TrustedExecutionTechnologyEnabled", nulls = AS_EMPTY)
            private Ref<Boolean> trustedExecutionTechnologyEnabled = unassigned();
            @JsonProperty(value = "PerformanceConfiguration")
            private PerformanceConfiguration performanceConfiguration = new PerformanceConfiguration();

            public class PerformanceConfiguration {
                @JsonProperty("Configurations")
                private List<PerformanceConfigurationObject> configurations = new ArrayList<>();

                public List<PerformanceConfigurationObject> getConfigurations() {
                    return configurations;
                }
            }
        }
    }

    public class Actions {
        @JsonProperty("#ComputerSystem.Reset")
        private Reset reset = new Reset();

        public class Reset {
            @JsonSetter(value = "ResetType@Redfish.AllowableValues", nulls = AS_EMPTY)
            @AsUnassigned({WHEN_NULL, WHEN_EMPTY_COLLECTION})
            private Ref<LinkedHashSet<ResetType>> allowableValues = unassigned();
        }
    }

    public class Links {
        @JsonProperty("Endpoints")
        private Set<ODataId> endpoints;
    }
}
