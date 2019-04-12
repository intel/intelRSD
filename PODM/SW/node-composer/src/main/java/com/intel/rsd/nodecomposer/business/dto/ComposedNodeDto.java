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

package com.intel.rsd.nodecomposer.business.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.nodecomposer.business.dto.actions.ResetActionDto;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.BootSourceMode;
import com.intel.rsd.nodecomposer.types.BootSourceState;
import com.intel.rsd.nodecomposer.types.ComposedNodeState;
import com.intel.rsd.nodecomposer.types.PowerState;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;
import lombok.Setter;

import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_ASSEMBLE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_ATTACH_RESOURCE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_DETACH_RESOURCE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_FORCE_DELETE;
import static com.intel.rsd.nodecomposer.types.actions.ComposedNodeActionNames.COMPOSED_NODE_RESET;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "uuid", "powerState", "status", "composedNodeState", "boot", "clearTpmOnDelete",
    "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ComposedNodeDto extends RedfishDto {
    @Getter
    @Setter
    @JsonProperty("UUID")
    private UUID uuid;

    @Getter
    @Setter
    private PowerState powerState;

    @Getter
    @Setter
    private Status status;

    @Getter
    @Setter
    private ComposedNodeState composedNodeState;

    @Getter
    private BootDto boot = new BootDto();

    @Getter
    @Setter
    @JsonProperty("ClearTPMOnDelete")
    private boolean clearTpmOnDelete;

    @Getter
    @Setter
    @JsonProperty("ClearOptaneDCPersistentMemoryOnDelete")
    private boolean clearOptanePersistentMemoryOnDelete;

    @Getter
    private Actions actions = new Actions();

    @Getter
    private Links links = new Links();

    @Getter
    private Oem oem = new Oem();

    public ComposedNodeDto() {
        super("#ComposedNode.v1_1_0.ComposedNode");
    }

    @JsonPropertyOrder({
        "bootSourceOverrideEnabled", "bootSourceOverrideTarget", "bootSourceOverrideTargetAllowableValues", "bootSourceOverrideMode",
        "bootSourceOverrideModeAllowableValues"
    })
    public static final class BootDto {
        private BootSourceState bootSourceOverrideEnabled;
        private String bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        private Collection<String> bootSourceOverrideTargetAllowableValues;
        private BootSourceMode bootSourceOverrideMode;
        @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
        private Collection<BootSourceMode> bootSourceOverrideModeAllowableValues;

        public BootSourceState getBootSourceOverrideEnabled() {
            return bootSourceOverrideEnabled;
        }

        public void setBootSourceOverrideEnabled(BootSourceState bootSourceOverrideEnabled) {
            this.bootSourceOverrideEnabled = bootSourceOverrideEnabled;
        }

        public String getBootSourceOverrideTarget() {
            return bootSourceOverrideTarget;
        }

        public void setBootSourceOverrideTarget(String bootSourceOverrideTarget) {
            this.bootSourceOverrideTarget = bootSourceOverrideTarget;
        }

        public Collection<String> getBootSourceOverrideTargetAllowableValues() {
            return bootSourceOverrideTargetAllowableValues;
        }

        public void setBootSourceOverrideTargetAllowableValues(Collection<String> bootSourceOverrideTargetAllowableValues) {
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

    @Getter
    @Setter
    @JsonPropertyOrder({"computerSystem", "processors", "memory", "ethernetInterfaces", "storage", "oem"})
    public final class Links {
        private ODataId computerSystem;
        private Set<ODataId> processors = new HashSet<>();
        private Set<ODataId> memory = new HashSet<>();
        private Set<ODataId> ethernetInterfaces = new HashSet<>();
        private Set<ODataId> storage = new HashSet<>();
        private Object oem = new Object();
    }

    @JsonPropertyOrder({"resetAction", "assembleAction", "attachResourceAction", "detachResourceAction"})
    public class Actions {
        @Getter
        @JsonProperty("#" + COMPOSED_NODE_RESET)
        private final ResetActionDto resetAction = new ResetActionDto();
        @Getter
        @JsonProperty("#" + COMPOSED_NODE_ASSEMBLE)
        private AssembleActionDto assembleAction = new AssembleActionDto();
        @Getter
        @JsonProperty("#" + COMPOSED_NODE_ATTACH_RESOURCE)
        private ResourceActionDto attachResourceAction = new ResourceActionDto();
        @Getter
        @JsonProperty("#" + COMPOSED_NODE_DETACH_RESOURCE)
        private ResourceActionDto detachResourceAction = new ResourceActionDto();
        @Getter
        @JsonProperty("#" + COMPOSED_NODE_FORCE_DELETE)
        private ForceDeleteActionDto forceDeleteAction = new ForceDeleteActionDto();

        public final class AssembleActionDto {
            @JsonProperty("target")
            private String target;

            public String getTarget() {
                return target;
            }

            public void setTarget(String target) {
                requiresNonNull(target, "target");
                this.target = target;
            }
        }

        @JsonPropertyOrder({"target", "@Redfish.ActionInfo"})
        public final class ResourceActionDto {
            @JsonProperty("target")
            private String target;

            @JsonProperty("@Redfish.ActionInfo")
            private String actionInfo;

            public String getTarget() {
                return target;
            }

            public void setTarget(String target) {
                requiresNonNull(target, "target");
                this.target = target;
            }

            public String getActionInfo() {
                return actionInfo;
            }

            public void setActionInfo(String target) {
                requiresNonNull(target, "actionInfo");
                this.actionInfo = target;
            }
        }

        @JsonPropertyOrder({"target", "ForceDelete"})
        public final class ForceDeleteActionDto {
            @JsonProperty("target")
            private String target;

            public String getTarget() {
                return target;
            }

            public void setTarget(String target) {
                requiresNonNull(target, "target");
                this.target = target;
            }
        }
    }

    @JsonPropertyOrder({"Intel_RackScale"})
    @JsonInclude(JsonInclude.Include.NON_NULL)
    public static final class Oem {
        @Setter
        @JsonProperty("Intel_RackScale")
        private IntelRackScale intelRackScale;

        @JsonPropertyOrder({"TaggedValues"})
        public static final class IntelRackScale {
            @Setter
            @JsonProperty("TaggedValues")
            Map<String, JsonNode> taggedValues;
        }
    }
}
