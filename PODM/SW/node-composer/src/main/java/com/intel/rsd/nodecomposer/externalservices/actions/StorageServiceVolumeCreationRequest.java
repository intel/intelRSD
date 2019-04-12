/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.ReplicaType;
import com.intel.rsd.nodecomposer.types.StorageAccessCapability;

import java.net.URI;
import java.util.List;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonInclude(NON_NULL)
public final class StorageServiceVolumeCreationRequest {
    private final Number capacityBytes;
    private String name;
    private String description;
    private Set<StorageAccessCapability> accessCapabilities;
    private List<CapacitySourceRequest> capacitySources;
    private List<ReplicaInfoRequest> replicaInfos;
    @JsonProperty("Oem")
    private Oem oem;

    public StorageServiceVolumeCreationRequest(Number capacityBytes) {
        this.capacityBytes = capacityBytes;
    }

    public Number getCapacityBytes() {
        return capacityBytes;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public Set<StorageAccessCapability> getAccessCapabilities() {
        return accessCapabilities;
    }

    public void setAccessCapabilities(Set<StorageAccessCapability> accessCapabilities) {
        this.accessCapabilities = accessCapabilities;
    }

    public List<CapacitySourceRequest> getCapacitySources() {
        return capacitySources;
    }

    public void setCapacitySources(List<CapacitySourceRequest> capacitySources) {
        this.capacitySources = capacitySources;
    }

    public List<ReplicaInfoRequest> getReplicaInfos() {
        return replicaInfos;
    }

    public void setReplicaInfos(List<ReplicaInfoRequest> replicaInfos) {
        this.replicaInfos = replicaInfos;
    }

    public void setBootable(Boolean bootable) {
        if (this.oem == null) {
            this.oem = new Oem();
        }

        this.oem.rackScaleOem.bootable = bootable;
    }

    public static class ReplicaInfoRequest {
        private ReplicaType replicaType;
        private ODataId replica;

        public ReplicaType getReplicaType() {
            return replicaType;
        }

        public void setReplicaType(ReplicaType replicaType) {
            this.replicaType = replicaType;
        }

        public ODataId getReplica() {
            return replica;
        }

        public void setReplica(URI uri) {
            this.replica = new ODataId(uri);
        }
    }

    public static class Oem {
        @JsonProperty("Intel_RackScale")
        private RackScaleOem rackScaleOem = new RackScaleOem();

        public class RackScaleOem {
            @JsonProperty("Bootable")
            private Boolean bootable;

            public Boolean getBootable() {
                return bootable;
            }

            public void setBootable(Boolean bootable) {
                this.bootable = bootable;
            }
        }
    }

    public static class CapacitySourceRequest {
        private List<ODataId> providingPools;

        public List<ODataId> getProvidingPools() {
            return providingPools;
        }

        public void setProvidingPools(List<ODataId> providingPools) {
            this.providingPools = providingPools;
        }
    }
}
