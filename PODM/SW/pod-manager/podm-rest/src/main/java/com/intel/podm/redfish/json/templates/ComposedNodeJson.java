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

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.redfish.json.templates.attributes.ResetActionJson;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "uuid", "powerState",
    "status", "processors", "memory", "composedNodeState", "boot", "oem", "links", "actions"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public final class ComposedNodeJson extends BaseJson {
    public String id;
    public String name;
    public String description;
    @JsonProperty("UUID")
    public UUID uuid;
    public PowerState powerState;
    public Status status;
    public final Processors processors = new Processors();
    public final Memory memory = new Memory();
    public ComposedNodeState composedNodeState;
    public final Boot boot = new Boot();

    public final Object oem = new Object();
    public final Links links = new Links();
    public final Actions actions = new Actions();

    public ComposedNodeJson() {
        super("#ComposedNode.v1_0_0.ComposedNode");
    }

    @JsonPropertyOrder({"count", "model", "status"})
    @JsonInclude(NON_NULL)
    public static final class Processors {
        public Integer count;
        public String model;
        public Status status;
    }

    @JsonPropertyOrder({"totalSystemMemoryGiB", "status"})
    @JsonInclude(NON_NULL)
    public static final class Memory {
        public BigDecimal totalSystemMemoryGiB;
        public Status status;
    }

    @JsonPropertyOrder({"bootSourceOverrideEnabled", "bootSourceOverrideTarget", "bootSourceAllowableValues"})
    @JsonInclude(NON_DEFAULT)
    public static final class Boot {
        public BootSourceState bootSourceOverrideEnabled;
        public BootSourceType bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        public final List<BootSourceType> bootSourceAllowableValues = new ArrayList<>();
        public BootSourceMode bootSourceOverrideMode;
        @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
        public List<BootSourceMode> bootSourceOverrideModeAllowableValues = new LinkedList<>();
    }

    @JsonPropertyOrder({"computerSystem", "processors", "memory", "ethernetInterfaces", "localDrives", "remoteDrives", "managedBy", "oem"})
    public static final class Links {
        public ODataId computerSystem;
        public final Set<ODataId> processors = new HashSet<>();
        public final Set<ODataId> memory = new HashSet<>();
        public final Set<ODataId> ethernetInterfaces = new HashSet<>();
        public final Set<ODataId> localDrives = new HashSet<>();
        public final Set<ODataId> remoteDrives = new HashSet<>();
        public final Set<ODataId> managedBy = new HashSet<>();
        public final Object oem = new Object();
    }

    @JsonPropertyOrder({"resetAction", "assembleAction", "attachEndpointAction", "detachEndpointAction"})
    public static final class Actions {
        @JsonProperty("#ComposedNode.Reset")
        public final ResetActionJson resetAction = new ResetActionJson();
        @JsonProperty("#ComposedNode.Assemble")
        public final AssembleActionJson assembleAction = new AssembleActionJson();
        @JsonProperty("#ComposedNode.AttachEndpoint")
        public final PcieDriveActionJson attachEndpointAction = new PcieDriveActionJson();
        @JsonProperty("#ComposedNode.DetachEndpoint")
        public final PcieDriveActionJson detachEndpointAction = new PcieDriveActionJson();

        public static final class AssembleActionJson {
            @JsonProperty("target")
            public String target;
        }

        @JsonPropertyOrder({"target", "allowableValues"})
        public static final class PcieDriveActionJson {
            @JsonProperty("target")
            public String target;
            @JsonProperty("Resource@Redfish.AllowableValues")
            public Set<ODataId> allowableValues = new HashSet<>();
        }
    }
}
