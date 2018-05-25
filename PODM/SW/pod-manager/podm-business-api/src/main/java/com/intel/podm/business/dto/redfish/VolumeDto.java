/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.dto.redfish;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.dto.DurableIdentifierDto;
import com.intel.podm.business.dto.RedfishDto;
import com.intel.podm.business.dto.actions.VolumeActionDto;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.SingletonContext;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.OemType;
import com.intel.podm.common.types.EncryptionTypes;
import com.intel.podm.common.types.StorageAccessCapability;
import com.intel.podm.common.types.VolumeType;

import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;
import static com.intel.podm.common.types.redfish.OemType.Type.OEM_IN_LINKS;
import static com.intel.podm.common.types.redfish.OemType.Type.TOP_LEVEL_OEM;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "status", "volumeType", "capacityBytes", "optimumIoSizeBytes",
    "blockSizeBytes", "capacity", "capacitySources", "encrypted", "encryptionTypes", "identifiers", "operations", "accessCapabilities", "manufacturer",
    "model", "replicaInfos", "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class VolumeDto extends RedfishDto {
    private final Links links = new Links();
    private final Actions actions = new Actions();
    private final Oem oem = new Oem();
    private Status status;
    private BigDecimal capacityBytes;
    private CapacityDto capacity;
    private List<CapacitySourceDto> capacitySources = new ArrayList<>();
    private VolumeType volumeType;
    private Boolean encrypted;
    @JsonInclude(NON_NULL)
    private EncryptionTypes encryptionTypes;
    private List<DurableIdentifierDto> identifiers = new ArrayList<>();
    private BigDecimal blockSizeBytes;
    private List<OperationDto> operations = new ArrayList<>();
    @JsonProperty("OptimumIOSizeBytes")
    private BigDecimal optimumIoSizeBytes;
    private List<StorageAccessCapability> accessCapabilities = new ArrayList<>();
    private List<ReplicaInfoDto> replicaInfos = new ArrayList<>();
    private String manufacturer;
    private String model;

    public VolumeDto() {
        super("#Volume.v1_1_0.Volume");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public BigDecimal getCapacityBytes() {
        return capacityBytes;
    }

    public void setCapacityBytes(BigDecimal capacityBytes) {
        this.capacityBytes = capacityBytes;
    }

    public CapacityDto getCapacity() {
        return capacity;
    }

    public void setCapacity(CapacityDto capacity) {
        this.capacity = capacity;
    }

    public List<CapacitySourceDto> getCapacitySources() {
        return capacitySources;
    }

    public void setCapacitySources(List<CapacitySourceDto> capacitySources) {
        this.capacitySources = capacitySources;
    }

    public List<ReplicaInfoDto> getReplicaInfos() {
        return replicaInfos;
    }

    public void setReplicaInfos(List<ReplicaInfoDto> replicaInfos) {
        this.replicaInfos = replicaInfos;
    }

    public VolumeType getVolumeType() {
        return volumeType;
    }

    public void setVolumeType(VolumeType volumeType) {
        this.volumeType = volumeType;
    }

    public Boolean getEncrypted() {
        return encrypted;
    }

    public void setEncrypted(Boolean encrypted) {
        this.encrypted = encrypted;
    }

    public EncryptionTypes getEncryptionTypes() {
        return encryptionTypes;
    }

    public void setEncryptionTypes(EncryptionTypes encryptionTypes) {
        this.encryptionTypes = encryptionTypes;
    }

    public List<DurableIdentifierDto> getIdentifiers() {
        return identifiers;
    }

    public void setIdentifiers(List<DurableIdentifierDto> identifiers) {
        this.identifiers = identifiers;
    }

    public BigDecimal getBlockSizeBytes() {
        return blockSizeBytes;
    }

    public void setBlockSizeBytes(BigDecimal blockSizeBytes) {
        this.blockSizeBytes = blockSizeBytes;
    }

    public List<OperationDto> getOperations() {
        return operations;
    }

    public void setOperations(List<OperationDto> operations) {
        this.operations = operations;
    }

    public BigDecimal getOptimumIoSizeBytes() {
        return optimumIoSizeBytes;
    }

    public void setOptimumIoSizeBytes(BigDecimal optimumIoSizeBytes) {
        this.optimumIoSizeBytes = optimumIoSizeBytes;
    }

    public List<StorageAccessCapability> getAccessCapabilities() {
        return accessCapabilities;
    }

    public void setAccessCapabilities(List<StorageAccessCapability> accessCapabilities) {
        this.accessCapabilities = accessCapabilities;
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

    public Links getLinks() {
        return links;
    }

    public Actions getActions() {
        return actions;
    }

    public Oem getOem() {
        return oem;
    }

    @JsonPropertyOrder({"drives", "oem"})
    public final class Links extends RedfishLinksDto {
        private Set<Context> drives = new HashSet<>();
        private final Oem oem = new Oem();

        public Set<Context> getDrives() {
            return drives;
        }

        public void setDrives(Set<Context> drives) {
            this.drives = drives;
        }

        public Oem getOem() {
            return oem;
        }

        @OemType(OEM_IN_LINKS)
        public final class Oem extends RedfishOemDto {
            @JsonProperty("Intel_RackScale")
            private RackScaleOem rackScaleOem = new RackScaleOem();

            public RackScaleOem getRackScaleOem() {
                return rackScaleOem;
            }

            @JsonPropertyOrder({"@odata.type", "endpoints"})
            public class RackScaleOem {
                @JsonProperty("@odata.type")
                private final String oDataType = "#Intel.Oem.VolumeLinks";
                private Set<Context> endpoints = new HashSet<>();

                public Set<Context> getEndpoints() {
                    return endpoints;
                }

                public void setEndpoints(Set<Context> endpoints) {
                    this.endpoints = endpoints;
                }
            }
        }
    }

    @JsonPropertyOrder({"initializeAction", "oem"})
    public class Actions extends RedfishActionsDto {
        @JsonProperty("#Volume.Initialize")
        private VolumeActionDto initializeAction;

        public VolumeActionDto getInitializeAction() {
            return initializeAction;
        }

        public void setInitializeAction(VolumeActionDto initializeAction) {
            this.initializeAction = initializeAction;
        }
    }

    @OemType(TOP_LEVEL_OEM)
    public class Oem extends RedfishOemDto {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public RackScaleOem getRackScaleOem() {
            return rackScaleOem;
        }

        @JsonPropertyOrder({"@odata.type", "bootable", "eraseOnDetach", "erased", "volumeMetrics"})
        public class RackScaleOem {
            @JsonProperty("@odata.type")
            private final String odataType = "#Intel.Oem.Volume";
            private Boolean bootable;
            private Boolean eraseOnDetach;
            private Boolean erased;
            @JsonInclude(NON_NULL)
            @JsonProperty("Metrics")
            private SingletonContext volumeMetrics;

            public String getOdataType() {
                return odataType;
            }

            public Boolean getBootable() {
                return bootable;
            }

            public void setBootable(Boolean bootable) {
                this.bootable = bootable;
            }

            public Boolean getEraseOnDetach() {
                return eraseOnDetach;
            }

            public void setEraseOnDetach(Boolean eraseOnDetach) {
                this.eraseOnDetach = eraseOnDetach;
            }

            public Boolean getErased() {
                return erased;
            }

            public void setErased(Boolean erased) {
                this.erased = erased;
            }

            public SingletonContext getVolumeMetrics() {
                return volumeMetrics;
            }

            public void setVolumeMetrics(SingletonContext volumeMetrics) {
                this.volumeMetrics = volumeMetrics;
            }
        }
    }
}
