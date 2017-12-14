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
import com.intel.podm.common.types.redfish.OemType;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.business.services.redfish.odataid.ODataIdFromSingletonContextHelper.asOdataId;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "uuid", "powerState",
    "status", "processors", "memory", "composedNodeState", "boot", "oem", "links", "actions"
})
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ExecutableStatementCount"})
public final class ComposedNodeDto extends RedfishDto {
    @JsonProperty("UUID")
    private UUID uuid;
    private PowerState powerState;
    private Status status;
    private ComputerSystemDto.ProcessorSummaryDto processors = new ComputerSystemDto.ProcessorSummaryDto();
    private ComputerSystemDto.MemorySummaryDto memory = new ComputerSystemDto.MemorySummaryDto();
    private ComposedNodeState composedNodeState;
    private BootDto boot = new BootDto();
    @IgnoreAutomaticOem
    private Actions actions = new Actions();
    private Links links = new Links();

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

    public ComputerSystemDto.ProcessorSummaryDto getProcessors() {
        return processors;
    }

    public void setProcessors(ComputerSystemDto.ProcessorSummaryDto processors) {
        this.processors = processors;
    }

    public ComputerSystemDto.MemorySummaryDto getMemory() {
        return memory;
    }

    public void setMemory(ComputerSystemDto.MemorySummaryDto memory) {
        this.memory = memory;
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

    public Actions getActions() {
        return actions;
    }

    public void setActions(Actions actions) {
        this.actions = actions;
    }

    public Links getLinks() {
        return links;
    }

    public void setLinks(Links links) {
        this.links = links;
    }

    @JsonPropertyOrder({"computerSystem", "processors", "memory", "ethernetInterfaces", "localDrives", "remoteDrives", "managedBy", "oem"})
    public final class Links extends RedfishLinksDto {
        private Context computerSystem;
        private Set<Context> processors = new HashSet<>();
        private Set<Context> memory = new HashSet<>();
        private Set<Context> ethernetInterfaces = new HashSet<>();
        private Set<Context> localDrives = new HashSet<>();
        private Set<Context> remoteDrives = new HashSet<>();
        private Set<Context> managedBy = new HashSet<>();
        private Oem oem = new Oem();

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

        public Set<Context> getLocalDrives() {
            return localDrives;
        }

        public void setLocalDrives(Set<Context> localDrives) {
            this.localDrives = localDrives;
        }

        public Set<Context> getRemoteDrives() {
            return remoteDrives;
        }

        public void setRemoteDrives(Set<Context> remoteDrives) {
            this.remoteDrives = remoteDrives;
        }

        public Set<Context> getManagedBy() {
            return managedBy;
        }

        public void setManagedBy(Set<Context> managedBy) {
            this.managedBy = managedBy;
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

    @JsonPropertyOrder({
        "@odata.type", "bootSourceOverrideEnabled",
        "bootSourceOverrideTarget", "bootSourceOverrideTargetAllowableValues",
        "bootSourceOverrideMode", "bootSourceOverrideModeAllowableValues"
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

    @JsonPropertyOrder({"resetAction", "assembleAction", "attachEndpointAction", "detachEndpointAction"})
    public class Actions {
        @JsonProperty("#ComposedNode.Reset")
        private final ResetActionDto resetAction = new ResetActionDto();
        @JsonProperty("#ComposedNode.Assemble")
        private AssembleActionDto assembleAction = new AssembleActionDto();
        @JsonProperty("#ComposedNode.AttachEndpoint")
        private PcieDriveActionDto attachEndpointAction = new PcieDriveActionDto();
        @JsonProperty("#ComposedNode.DetachEndpoint")
        private PcieDriveActionDto detachEndpointAction = new PcieDriveActionDto();

        public ResetActionDto getResetAction() {
            return resetAction;
        }

        public AssembleActionDto getAssembleAction() {
            return assembleAction;
        }

        public void setAssembleAction(AssembleActionDto assembleAction) {
            this.assembleAction = assembleAction;
        }

        public PcieDriveActionDto getAttachEndpointAction() {
            return attachEndpointAction;
        }

        public void setAttachEndpointAction(PcieDriveActionDto attachEndpointAction) {
            this.attachEndpointAction = attachEndpointAction;
        }

        public PcieDriveActionDto getDetachEndpointAction() {
            return detachEndpointAction;
        }

        public void setDetachEndpointAction(PcieDriveActionDto detachEndpointAction) {
            this.detachEndpointAction = detachEndpointAction;
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

        @JsonPropertyOrder({"target", "allowableValues"})
        public final class PcieDriveActionDto {
            @JsonProperty("target")
            private String target;
            @JsonProperty("Resource@Redfish.AllowableValues")
            private Set<ODataId> allowableValues = new HashSet<>();

            public String getTarget() {
                return target;
            }

            public void setTarget(SingletonContext target) {
                requiresNonNull(target, "target");
                this.target = asOdataId(target).toString();
            }

            public Set<ODataId> getAllowableValues() {
                return allowableValues;
            }

            public void setAllowableValues(Set<ODataId> allowableValues) {
                this.allowableValues = allowableValues;
            }
        }
    }
}
