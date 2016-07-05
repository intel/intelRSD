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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.NodeSystemType;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.rest.odataid.ODataId;

import java.util.Collection;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.google.common.collect.Lists.newArrayList;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "systemType", "assetTag",
        "manufacturer", "model", "sku", "serialNumber", "partNumber", "uuid", "hostName", "powerState",
        "biosVersion", "status", "processors", "memory", "composedNodeState", "boot", "oem", "links", "actions"
})
public final class ComposedNodeJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
    public NodeSystemType systemType;
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
    public PowerState powerState;
    public String biosVersion;
    public Status status;
    public final Processors processors = new Processors();
    public final Memory memory = new Memory();
    public ComposedNodeState composedNodeState;
    public final Boot boot = new Boot();

    public final Object oem = new Object();
    public final Links links = new Links();
    public final Actions actions = new Actions();

    public ComposedNodeJson() {
        super("#ComposedNode.1.0.0.ComposedNode");
    }

    @JsonPropertyOrder({"count", "model", "status"})
    @JsonInclude(NON_NULL)
    public static final class Processors {
        public int count;
        public String model;
        public Status status;
    }

    @JsonPropertyOrder({"totalSystemMemoryGiB", "status"})
    @JsonInclude(NON_NULL)
    public static final class Memory {
        public Integer totalSystemMemoryGiB;
        public Status status;
    }

    @JsonPropertyOrder({"bootSourceOverrideEnabled", "bootSourceOverrideTarget", "bootSourceAllowableValues"})
    @JsonInclude(NON_DEFAULT)
    public static final class Boot {
        public BootSourceState bootSourceOverrideEnabled;
        public BootSourceType bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        public final Collection<BootSourceType> bootSourceAllowableValues = newArrayList();
    }

    @JsonPropertyOrder({"computerSystem", "processors", "memory", "ethernetInterfaces", "localDrives", "remoteDrives", "managedBy", "oem"})
    public static final class Links extends BaseLinksJson {
        public ODataId computerSystem;
        public final Collection<ODataId> processors = newArrayList();
        public final Collection<ODataId> memory = newArrayList();
        public final Collection<ODataId> ethernetInterfaces = newArrayList();
        public final Collection<ODataId> localDrives = newArrayList();
        public final Collection<ODataId> remoteDrives = newArrayList();
        public final Collection<ODataId> managedBy = newArrayList();
        public final Object oem = new Object();
    }

    @JsonPropertyOrder({"resetAction"})
    public static final class Actions {
        @JsonProperty("#ComposedNode.Reset")
        public final ResetActionJson resetAction = new ResetActionJson();
        @JsonProperty("#ComposedNode.Assemble")
        public final AssembleActionJson assembleAction = new AssembleActionJson();

        @JsonPropertyOrder({"target", "resetType"})
        public static final class ResetActionJson {
            @JsonProperty("target")
            public String target;
            @JsonProperty("ResetType@DMTF.AllowableValues")
            public final Collection<ResetType> allowableResetTypes = newArrayList();
        }

        @JsonPropertyOrder({"target"})
        public static final class AssembleActionJson {
            @JsonProperty("target")
            public String target;
        }
    }
}
