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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.actions.ActionDto;
import com.intel.podm.business.dto.actions.ChangeTpmStateActionDto;
import com.intel.podm.business.dto.actions.ResetActionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.InterfaceType;
import com.intel.podm.common.types.InterfaceTypeSelection;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;
import com.intel.podm.common.types.redfish.OemType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "systemType", "assetTag", "manufacturer", "model",
    "sku", "serialNumber", "partNumber", "uuid", "hostName", "status", "indicatorLed", "powerState", "boot", "biosVersion",
    "processorSummary", "memorySummary", "processors", "ethernetInterfaces", "simpleStorage", "storage", "memory",
    "pcieDevices", "pcieDeviceFunctions", "networkInterfaces", "links", "trustedModules", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public final class ComputerSystemDto extends RedfishDto {
    private SystemType systemType;
    private String assetTag;
    private String manufacturer;
    private String model;
    @JsonProperty("SKU")
    private String sku;
    private String serialNumber;
    private String partNumber;
    @JsonProperty("UUID")
    private UUID uuid;
    private String hostName;
    private Status status;
    @JsonProperty("IndicatorLED")
    private IndicatorLed indicatorLed;
    private PowerState powerState;
    private BootDto boot = new BootDto();
    private String biosVersion;
    private ProcessorSummaryDto processorSummary = new ProcessorSummaryDto();
    private MemorySummaryDto memorySummary = new MemorySummaryDto();
    private SingletonContext processors;
    private SingletonContext ethernetInterfaces;
    private SingletonContext simpleStorage;
    private SingletonContext storage;
    private SingletonContext memory;
    @JsonProperty("PCIeDevices")
    private Set<Context> pcieDevices = new HashSet<>();
    @JsonProperty("PCIeFunctions")
    private Set<Context> pcieDeviceFunctions = new HashSet<>();
    private SingletonContext networkInterfaces;
    private List<TrustedModuleDto> trustedModules = new ArrayList<>();
    private Links links = new Links();
    private Actions actions = new Actions();
    private Oem oem = new Oem();

    public ComputerSystemDto() {
        super("#ComputerSystem.v1_3_0.ComputerSystem");
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

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public String getHostName() {
        return hostName;
    }

    public void setHostName(String hostName) {
        this.hostName = hostName;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public IndicatorLed getIndicatorLed() {
        return indicatorLed;
    }

    public void setIndicatorLed(IndicatorLed indicatorLed) {
        this.indicatorLed = indicatorLed;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public BootDto getBoot() {
        return boot;
    }

    public void setBoot(BootDto boot) {
        this.boot = boot;
    }

    public String getBiosVersion() {
        return biosVersion;
    }

    public void setBiosVersion(String biosVersion) {
        this.biosVersion = biosVersion;
    }

    public ProcessorSummaryDto getProcessorSummary() {
        return processorSummary;
    }

    public void setProcessorSummary(ProcessorSummaryDto processorSummary) {
        this.processorSummary = processorSummary;
    }

    public MemorySummaryDto getMemorySummary() {
        return memorySummary;
    }

    public void setMemorySummary(MemorySummaryDto memorySummary) {
        this.memorySummary = memorySummary;
    }

    public SingletonContext getProcessors() {
        return processors;
    }

    public void setProcessors(SingletonContext processors) {
        this.processors = processors;
    }

    public SingletonContext getEthernetInterfaces() {
        return ethernetInterfaces;
    }

    public void setEthernetInterfaces(SingletonContext ethernetInterfaces) {
        this.ethernetInterfaces = ethernetInterfaces;
    }

    public SingletonContext getSimpleStorage() {
        return simpleStorage;
    }

    public void setSimpleStorage(SingletonContext simpleStorage) {
        this.simpleStorage = simpleStorage;
    }

    public SingletonContext getStorage() {
        return storage;
    }

    public void setStorage(SingletonContext storage) {
        this.storage = storage;
    }

    public SingletonContext getMemory() {
        return memory;
    }

    public void setMemory(SingletonContext memory) {
        this.memory = memory;
    }

    public Set<Context> getPcieDevices() {
        return pcieDevices;
    }

    public void setPcieDevices(Set<Context> pcieDevices) {
        this.pcieDevices = pcieDevices;
    }

    public Set<Context> getPcieDeviceFunctions() {
        return pcieDeviceFunctions;
    }

    public void setPcieDeviceFunctions(Set<Context> pcieDeviceFunctions) {
        this.pcieDeviceFunctions = pcieDeviceFunctions;
    }

    public SingletonContext getNetworkInterfaces() {
        return networkInterfaces;
    }

    public void setNetworkInterfaces(SingletonContext networkInterfaces) {
        this.networkInterfaces = networkInterfaces;
    }

    public List<TrustedModuleDto> getTrustedModules() {
        return trustedModules;
    }

    public void setTrustedModules(List<TrustedModuleDto> trustedModules) {
        this.trustedModules = trustedModules;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    public Actions getActions() {
        return actions;
    }

    public void setActions(Actions actions) {
        this.actions = actions;
    }

    public Oem getOem() {
        return oem;
    }

    public void setOem(Oem oem) {
        this.oem = oem;
    }

    @JsonPropertyOrder({"totalSystemMemoryGiB", "status"})
    public static final class MemorySummaryDto {
        @JsonProperty("TotalSystemMemoryGiB")
        private BigDecimal totalSystemMemoryGiB;
        @JsonProperty("Status")
        private Status status;

        public BigDecimal getTotalSystemMemoryGiB() {
            return totalSystemMemoryGiB;
        }

        public void setTotalSystemMemoryGiB(BigDecimal totalSystemMemoryGiB) {
            this.totalSystemMemoryGiB = totalSystemMemoryGiB;
        }

        public Status getStatus() {
            return status;
        }

        public void setStatus(Status status) {
            this.status = status;
        }
    }

    @JsonPropertyOrder({"count", "model", "status"})
    public static final class ProcessorSummaryDto {
        private Integer count;
        private String model;
        private Status status;

        public Integer getCount() {
            return count;
        }

        public void setCount(Integer count) {
            this.count = count;
        }

        public String getModel() {
            return model;
        }

        public void setModel(String model) {
            this.model = model;
        }

        public Status getStatus() {
            return status;
        }

        public void setStatus(Status status) {
            this.status = status;
        }
    }

    @JsonPropertyOrder({
        "@odata.type", "bootSourceOverrideEnabled",
        "bootSourceOverrideTarget", "bootSourceOverrideTargetAllowableValues",
        "bootSourceOverrideMode", "bootSourceOverrideModeAllowableValues"
    })
    public static final class BootDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#ComputerSystem.v1_1_0.Boot";
        private BootSourceState bootSourceOverrideEnabled;
        private BootSourceType bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        private Collection<BootSourceType> bootSourceOverrideTargetAllowableValues;
        private BootSourceMode bootSourceOverrideMode;
        @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
        private Collection<BootSourceMode> bootSourceOverrideModeAllowableValues;

        public String getoDataType() {
            return oDataType;
        }

        public BootSourceState getBootSourceOverrideEnabled() {
            return bootSourceOverrideEnabled;
        }

        public void setBootSourceOverrideEnabled(BootSourceState bootSourceOverrideEnabled) {
            this.bootSourceOverrideEnabled = bootSourceOverrideEnabled;
        }

        public BootSourceType getBootSourceOverrideTarget() {
            return bootSourceOverrideTarget;
        }

        public void setBootSourceOverrideTarget(BootSourceType bootSourceOverrideTarget) {
            this.bootSourceOverrideTarget = bootSourceOverrideTarget;
        }

        public Collection<BootSourceType> getBootSourceOverrideTargetAllowableValues() {
            return bootSourceOverrideTargetAllowableValues;
        }

        public void setBootSourceOverrideTargetAllowableValues(Collection<BootSourceType> bootSourceOverrideTargetAllowableValues) {
            this.bootSourceOverrideTargetAllowableValues = bootSourceOverrideTargetAllowableValues;
        }

        public BootSourceMode getBootSourceOverrideMode() {
            return bootSourceOverrideMode;
        }

        public void setBootSourceOverrideMode(BootSourceMode bootSourceOverrideMode) {
            this.bootSourceOverrideMode = bootSourceOverrideMode;
        }

        public Collection<BootSourceMode> getBootSourceOverrideModeAllowableValues() {
            return bootSourceOverrideModeAllowableValues;
        }

        public void setBootSourceOverrideModeAllowableValues(Collection<BootSourceMode> bootSourceOverrideModeAllowableValues) {
            this.bootSourceOverrideModeAllowableValues = bootSourceOverrideModeAllowableValues;
        }
    }

    @JsonPropertyOrder({"@odata.type", "firmwareVersion", "firmwareVersion2", "interfaceType", "interfaceTypeSelection", "status", "oem"})
    public static final class TrustedModuleDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#ComputerSystem.v1_3_0.TrustedModules";
        private String firmwareVersion;
        private String firmwareVersion2;
        private InterfaceType interfaceType;
        private InterfaceTypeSelection interfaceTypeSelection;
        private Status status;
        @IgnoreAutomaticOem
        private Object oem = new Object();

        public TrustedModuleDto() {
        }

        public String getoDataType() {
            return oDataType;
        }

        public String getFirmwareVersion() {
            return firmwareVersion;
        }

        public void setFirmwareVersion(String firmwareVersion) {
            this.firmwareVersion = firmwareVersion;
        }

        public String getFirmwareVersion2() {
            return firmwareVersion2;
        }

        public void setFirmwareVersion2(String firmwareVersion2) {
            this.firmwareVersion2 = firmwareVersion2;
        }

        public InterfaceType getInterfaceType() {
            return interfaceType;
        }

        public void setInterfaceType(InterfaceType interfaceType) {
            this.interfaceType = interfaceType;
        }

        public InterfaceTypeSelection getInterfaceTypeSelection() {
            return interfaceTypeSelection;
        }

        public void setInterfaceTypeSelection(InterfaceTypeSelection interfaceTypeSelection) {
            this.interfaceTypeSelection = interfaceTypeSelection;
        }

        public Status getStatus() {
            return status;
        }

        public void setStatus(Status status) {
            this.status = status;
        }

        public Object getOem() {
            return oem;
        }

        public void setOem(Object oem) {
            this.oem = oem;
        }
    }

    @JsonPropertyOrder({"vendorId", "deviceId"})
    public static final class OemDeviceDto {
        @JsonProperty("VendorId")
        private String vendorId;
        @JsonProperty("DeviceId")
        private String deviceId;

        public String getVendorId() {
            return vendorId;
        }

        public void setVendorId(String vendorId) {
            this.vendorId = vendorId;
        }

        public String getDeviceId() {
            return deviceId;
        }

        public void setDeviceId(String deviceId) {
            this.deviceId = deviceId;
        }
    }

    @JsonPropertyOrder({"@odata.type", "chassis", "managedBy", "endpoints", "oem"})
    public final class Links extends RedfishLinksDto {
        @JsonProperty("@odata.type")
        private final String oDataType = "#ComputerSystem.v1_2_0.Links";
        private Set<Context> chassis = new HashSet<>();
        private Set<Context> managedBy = new HashSet<>();
        private Set<Context> endpoints = new HashSet<>();
        private Oem oem = new Oem();

        public String getoDataType() {
            return oDataType;
        }

        public Set<Context> getChassis() {
            return chassis;
        }

        public void setChassis(Set<Context> chassis) {
            this.chassis = chassis;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
        }

        public Set<Context> getEndpoints() {
            return endpoints;
        }

        public void setEndpoints(Set<Context> endpoints) {
            this.endpoints = endpoints;
        }

        public Oem getOem() {
            return oem;
        }

        public void setOem(Oem oem) {
            this.oem = oem;
        }

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemDto {
        }
    }

    @JsonPropertyOrder({"reset", "oem"})
    public class Actions extends RedfishActionsDto {
        @JsonProperty("#ComputerSystem.Reset")
        private final ResetActionDto reset = new ResetActionDto();
        @JsonProperty("Oem")
        private Oem oem = new Oem();

        public ResetActionDto getReset() {
            return reset;
        }

        public Oem getOem() {
            return oem;
        }

        @OemType(OEM_IN_ACTIONS)
        public class Oem extends RedfishOemDto {
            @JsonProperty("#Intel.Oem.StartDeepDiscovery")
            private final ActionDto startDeepDiscovery = new ActionDto();

            @JsonProperty("#Intel.Oem.ChangeTPMState")
            private final ChangeTpmStateActionDto changeTpmState = new ChangeTpmStateActionDto();

            public ActionDto getStartDeepDiscovery() {
                return startDeepDiscovery;
            }

            public ChangeTpmStateActionDto getChangeTpmState() {
                return changeTpmState;
            }
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private final RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({
            "odataType", "pciDevices", "pcieConnectionId", "discoveryState", "processorSockets", "memorySockets",
            "pcieConnectionId", "userModeEnabled", "trustedExecutionTechnologyEnabled", "computerSystemMetrics"
        })
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String odataType = "#Intel.Oem.ComputerSystem";
            private List<OemDeviceDto> pciDevices = new ArrayList<>();
            private DiscoveryState discoveryState;
            private Integer processorSockets;
            private Integer memorySockets;
            @JsonProperty("PCIeConnectionId")
            private List<String> pcieConnectionId;
            private Boolean userModeEnabled;
            private Boolean trustedExecutionTechnologyEnabled;
            @JsonProperty("Metrics")
            @JsonInclude(NON_NULL)
            private SingletonContext computerSystemMetrics;

            public String getOdataType() {
                return odataType;
            }

            public List<OemDeviceDto> getPciDevices() {
                return pciDevices;
            }

            public void setPciDevices(List<OemDeviceDto> pciDevices) {
                this.pciDevices = pciDevices;
            }

            public DiscoveryState getDiscoveryState() {
                return discoveryState;
            }

            public void setDiscoveryState(DiscoveryState discoveryState) {
                this.discoveryState = discoveryState;
            }

            public Integer getProcessorSockets() {
                return processorSockets;
            }

            public void setProcessorSockets(Integer processorSockets) {
                this.processorSockets = processorSockets;
            }

            public Integer getMemorySockets() {
                return memorySockets;
            }

            public void setMemorySockets(Integer memorySockets) {
                this.memorySockets = memorySockets;
            }

            public List<String> getPcieConnectionId() {
                return pcieConnectionId;
            }

            public void setPcieConnectionId(List<String> pcieConnectionId) {
                this.pcieConnectionId = pcieConnectionId;
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

            public SingletonContext getComputerSystemMetrics() {
                return computerSystemMetrics;
            }

            public void setComputerSystemMetrics(SingletonContext computerSystemMetrics) {
                this.computerSystemMetrics = computerSystemMetrics;
            }
        }
    }
}
