/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.DiscoveryState;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.redfish.json.templates.attributes.ComputerSystemDeviceJson;
import com.intel.podm.redfish.json.templates.attributes.ResetActionJson;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_ACTIONS;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "systemType", "assetTag", "manufacturer", "model",
    "sku", "serialNumber", "partNumber", "description", "uuid", "hostName", "status", "indicatorLed", "powerState",
    "boot", "biosVersion", "processorSummary", "memorySummary", "processors", "ethernetInterfaces", "simpleStorage",
    "storage", "memory", "pcieDevices", "pcieDeviceFunctions", "networkInterfaces", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:VisibilityModifier"})
public class ComputerSystemJson extends BaseResourceJson {
    public SystemType systemType;
    public String assetTag;
    public String manufacturer;
    public String model;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    @JsonProperty("UUID")
    public UUID uuid;
    public String hostName;
    public Status status;
    @JsonProperty("IndicatorLED")
    public IndicatorLed indicatorLed;
    public PowerState powerState;
    public Boot boot = new Boot();
    public String biosVersion;
    public ProcessorSummary processorSummary = new ProcessorSummary();
    public MemorySummary memorySummary = new MemorySummary();
    public ODataId processors;
    public ODataId ethernetInterfaces;
    public ODataId simpleStorage;
    public ODataId storage;
    public ODataId memory;
    @JsonProperty("PCIeDevices")
    public Set<ODataId> pcieDevices = new HashSet<>();
    @JsonProperty("PCIeFunctions")
    public Set<ODataId> pcieDeviceFunctions = new HashSet<>();
    public ODataId networkInterfaces;
    public Links links = new Links();
    public Actions actions = new Actions();
    public Oem oem = new Oem();

    public ComputerSystemJson() {
        super("#ComputerSystem.v1_3_0.ComputerSystem");
    }

    @JsonPropertyOrder({"count", "model", "status"})
    public static final class ProcessorSummary {
        public Integer count;
        public String model;
        public Status status;
    }

    @JsonPropertyOrder({"totalSystemMemoryGiB", "status"})
    public static final class MemorySummary {
        public BigDecimal totalSystemMemoryGiB;
        public Status status;
    }

    @JsonPropertyOrder({"@odata.type", "bootSourceOverrideEnabled", "bootSourceOverrideTarget", "bootSourceAllowableValues",
        "bootSourceOverrideMode", "bootSourceOverrideModeAllowableValues"})
    public static class Boot {
        @JsonProperty("@odata.type")
        public String oDataType;
        public BootSourceState bootSourceOverrideEnabled;
        public BootSourceType bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        public List<BootSourceType> bootSourceAllowableValues = new LinkedList<>();
        public BootSourceMode bootSourceOverrideMode;
        @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
        public List<BootSourceMode> bootSourceOverrideModeAllowableValues = new LinkedList<>();
    }

    @JsonPropertyOrder({"@odata.type", "chassis", "managedBy", "endpoints", "oem"})
    public class Links extends RedfishLinksJson {
        @JsonProperty("@odata.type")
        public final String oDataType = "#ComputerSystem.v1_2_0.Links";
        public Set<ODataId> chassis = new HashSet<>();
        public Set<ODataId> managedBy = new HashSet<>();
        public Set<ODataId> endpoints = new HashSet<>();
        public Oem oem = new Oem();

        @OemType(OEM_IN_LINKS)
        public class Oem extends RedfishOemJson {
        }
    }

    @JsonPropertyOrder({"reset", "oem"})
    public class Actions extends RedfishActionsJson {
        @JsonProperty("#ComputerSystem.Reset")
        public ResetActionJson reset = new ResetActionJson();
        @JsonProperty("Oem")
        public Oem oem = new Oem();

        @OemType(OEM_IN_ACTIONS)
        public class Oem extends RedfishOemJson {
            @JsonProperty("#Intel.Oem.StartDeepDiscovery")
            public StartDeepDiscoveryAction startDeepDiscovery = new StartDeepDiscoveryAction();

            public class StartDeepDiscoveryAction {
                @JsonProperty("target")
                public String target;
            }
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemJson {
        @JsonProperty("Intel_RackScale")
        public RackScaleOem rackScaleOem = new RackScaleOem();

        @JsonPropertyOrder({
            "odataType", "pciDevices", "pcieConnectionId", "discoveryState", "processorSockets", "memorySockets"
        })
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            public String odataType;
            public List<ComputerSystemDeviceJson> pciDevices = new ArrayList<>();
            public DiscoveryState discoveryState;
            public Integer processorSockets;
            public Integer memorySockets;
            @JsonProperty("PCIeConnectionId")
            public List<String> pcieConnectionId;
        }
    }
}
