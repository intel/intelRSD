/*
 * Copyright (c) 2015 Intel Corporation
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
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.IndicatorLed;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.SystemType;
import com.intel.podm.redfish.json.templates.actions.ResetActionJson;
import com.intel.podm.redfish.json.templates.actions.StartDeepDiscoveryActionJson;
import com.intel.podm.redfish.json.templates.attributes.SystemOemJson;
import com.intel.podm.rest.odataid.ODataId;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.UUID;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "systemType", "assetTag", "manufacturer", "model",
        "sku", "serialNumber", "partNumber", "description", "uuid", "hostName", "status", "indicatorLed", "powerState",
        "boot", "biosVersion", "processorSummary", "memorySummary", "processors", "ethernetInterfaces", "simpleStorage",
        "memory", "memoryChunks", "links", "actions", "oem"
})
public class ComputerSystemJson extends BaseJson {

    public Id id;
    public String name;
    public SystemType systemType;
    public String assetTag;
    public String manufacturer;
    public String model;
    @JsonProperty("SKU")
    public String sku;
    public String serialNumber;
    public String partNumber;
    public String description;
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
    public ODataId memory;
    public ODataId memoryChunks;
    public Links links = new Links();
    public Actions actions = new Actions();
    public SystemOemJson oem = new SystemOemJson();

    public ComputerSystemJson(String oDataType) {
        super(oDataType);
    }

    @JsonPropertyOrder({"chassis", "managedBy", "oem"})
    public static final class Links {
        public List<ODataId> chassis = new ArrayList<>();
        public List<ODataId> managedBy = new ArrayList<>();
        public Object oem = new Object();
    }

    @JsonPropertyOrder({"count", "model", "status"})
    public static final class ProcessorSummary {
        public Integer count;
        public String model;
        public Status status;
    }

    @JsonPropertyOrder({"totalSystemMemoryGiB", "status"})
    public static final class MemorySummary {
        public Integer totalSystemMemoryGiB;
        public Status status;
    }

    @JsonPropertyOrder({"bootSourceOverrideEnabled", "bootSourceOverrideTarget", "bootSourceAllowableValues"})
    public static class Boot {
        public BootSourceState bootSourceOverrideEnabled;
        public BootSourceType bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        public List<BootSourceType> bootSourceAllowableValues = new LinkedList<>();
    }

    @JsonPropertyOrder({"resetActionJson", "oem"})
    public static class Actions {
        @JsonProperty("#ComputerSystem.Reset")
        public ResetActionJson resetActionJson = new ResetActionJson();

        public Oem oem = new Oem();

        public static class Oem {
            @JsonProperty("Intel_RackScale")
            public RackScaleOem rackScaleOem = new RackScaleOem();

            public static class RackScaleOem {
                @JsonProperty("#ComputerSystem.StartDeepDiscovery")
                public StartDeepDiscoveryActionJson startDeepDiscoveryActionJson = new StartDeepDiscoveryActionJson();
            }
        }
    }
}
