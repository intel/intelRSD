/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.actions.ResetActionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.BootSourceMode;
import com.intel.podm.common.types.BootSourceState;
import com.intel.podm.common.types.BootSourceType;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.PowerState;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromSingletonContextHelper.asOdataId;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "uuid", "powerState", "status", "composedNodeState",
    "boot", "clearTpmOnDelete", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class ComposedNodeDto extends RedfishDto {
    @IgnoreAutomaticOem
    private final Actions actions = new Actions();
    private final Links links = new Links();
    @JsonProperty("UUID")
    private UUID uuid;
    private PowerState powerState;
    private Status status;
    private ComposedNodeState composedNodeState;
    private BootDto boot = new BootDto();
    @JsonProperty("ClearTPMOnDelete")
    private boolean clearTpmOnDelete;

    public ComposedNodeDto() {
        super("#ComposedNode.v1_0_0.ComposedNode");
    }

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public PowerState getPowerState() {
        return powerState;
    }

    public void setPowerState(PowerState powerState) {
        this.powerState = powerState;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public ComposedNodeState getComposedNodeState() {
        return composedNodeState;
    }

    public void setComposedNodeState(ComposedNodeState composedNodeState) {
        this.composedNodeState = composedNodeState;
    }

    public BootDto getBoot() {
        return boot;
    }

    public void setBoot(BootDto boot) {
        this.boot = boot;
    }

    public boolean getClearTpmOnDelete() {
        return clearTpmOnDelete;
    }

    public void setClearTpmOnDelete(boolean clearTpmOnDelete) {
        this.clearTpmOnDelete = clearTpmOnDelete;
    }

    public Actions getActions() {
        return actions;
    }

    public Links getLinks() {
        return links;
    }

    @JsonPropertyOrder({
        "bootSourceOverrideEnabled", "bootSourceOverrideTarget", "bootSourceOverrideTargetAllowableValues", "bootSourceOverrideMode",
        "bootSourceOverrideModeAllowableValues"
    })
    public static final class BootDto {
        private BootSourceState bootSourceOverrideEnabled;
        private BootSourceType bootSourceOverrideTarget;
        @JsonProperty("BootSourceOverrideTarget@Redfish.AllowableValues")
        private Collection<BootSourceType> bootSourceOverrideTargetAllowableValues;
        private BootSourceMode bootSourceOverrideMode;
        @JsonProperty("BootSourceOverrideMode@Redfish.AllowableValues")
        private Collection<BootSourceMode> bootSourceOverrideModeAllowableValues;

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

    @JsonPropertyOrder({"computerSystem", "processors", "memory", "ethernetInterfaces", "storage", "managedBy", "oem"})
    public final class Links extends RedfishLinksDto {
        private Context computerSystem;
        private Set<Context> processors = new HashSet<>();
        private Set<Context> memory = new HashSet<>();
        private Set<Context> ethernetInterfaces = new HashSet<>();
        private Set<Context> storage = new HashSet<>();
        private Set<Context> managedBy = new HashSet<>();

        public Context getComputerSystem() {
            return computerSystem;
        }

        public void setComputerSystem(Context computerSystem) {
            this.computerSystem = computerSystem;
        }

        public Set<Context> getProcessors() {
            return processors;
        }

        public void setProcessors(Set<Context> processors) {
            this.processors = processors;
        }

        public Set<Context> getMemory() {
            return memory;
        }

        public void setMemory(Set<Context> memory) {
            this.memory = memory;
        }

        public Set<Context> getEthernetInterfaces() {
            return ethernetInterfaces;
        }

        public void setEthernetInterfaces(Set<Context> ethernetInterfaces) {
            this.ethernetInterfaces = ethernetInterfaces;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
        }

        public Set<Context> getStorage() {
            return storage;
        }

        public void setStorage(Set<Context> storage) {
            this.storage = storage;
        }
    }

    @JsonPropertyOrder({"resetAction", "assembleAction", "attachResourceAction", "detachResourceAction"})
    public class Actions {
        @JsonProperty("#ComposedNode.Reset")
        private final ResetActionDto resetAction = new ResetActionDto();
        @JsonProperty("#ComposedNode.Assemble")
        private AssembleActionDto assembleAction = new AssembleActionDto();
        @JsonProperty("#ComposedNode.AttachResource")
        private ResourceActionDto attachResourceAction = new ResourceActionDto();
        @JsonProperty("#ComposedNode.DetachResource")
        private ResourceActionDto detachResourceAction = new ResourceActionDto();

        public ResetActionDto getResetAction() {
            return resetAction;
        }

        public AssembleActionDto getAssembleAction() {
            return assembleAction;
        }

        public void setAssembleAction(AssembleActionDto assembleAction) {
            this.assembleAction = assembleAction;
        }

        public ResourceActionDto getAttachResourceAction() {
            return attachResourceAction;
        }

        public void setAttachResourceAction(ResourceActionDto attachResourceAction) {
            this.attachResourceAction = attachResourceAction;
        }

        public ResourceActionDto getDetachResourceAction() {
            return detachResourceAction;
        }

        public void setDetachResourceAction(ResourceActionDto detachResourceAction) {
            this.detachResourceAction = detachResourceAction;
        }

        public final class AssembleActionDto {
            @JsonProperty("target")
            private String target;

            public String getTarget() {
                return target;
            }

            public void setTarget(SingletonContext target) {
                requiresNonNull(target, "target");
                this.target = asOdataId(target).toString();
            }
        }

        @JsonPropertyOrder({"target", "@Redfish.ActionInfo"})
        public final class ResourceActionDto {
            @JsonProperty("target")
            private String target;

            @JsonProperty("@Redfish.ActionInfo")
            private ODataId actionInfo;

            public String getTarget() {
                return target;
            }

            public void setTarget(SingletonContext target) {
                requiresNonNull(target, "target");
                this.target = asOdataId(target).toString();
            }

            public ODataId getActionInfo() {
                return actionInfo;
            }

            public void setActionInfo(SingletonContext target) {
                requiresNonNull(target, "actionInfo");
                this.actionInfo = asOdataId(target);
            }
        }
    }
}
