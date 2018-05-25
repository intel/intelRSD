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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.ReplicaType;
import com.intel.podm.common.types.StorageAccessCapability;
import com.intel.podm.common.types.redfish.StorageServiceVolume;

import java.util.List;
import java.util.Set;

public final class CreateStorageServiceVolumeJson implements StorageServiceVolume {
    @JsonProperty("Name")
    private String name;
    @JsonProperty("Description")
    private String description;
    @JsonProperty("CapacityBytes")
    private Number capacityBytes;
    @JsonProperty("CapacitySources")
    private List<CapacitySourceJson> capacitySources;
    @JsonProperty("AccessCapabilities")
    private Set<StorageAccessCapability> accessCapabilities;
    @JsonProperty("ReplicaInfos")
    private List<ReplicaInfoJson> replicaInfos;

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public Number getCapacityBytes() {
        return capacityBytes;
    }

    @Override
    public List<CapacitySourceJson> getCapacitySources() {
        return capacitySources;
    }

    @Override
    public Set<StorageAccessCapability> getAccessCapabilities() {
        return accessCapabilities;
    }

    @Override
    public List<ReplicaInfoJson> getReplicaInfos() {
        return replicaInfos;
    }

    public static class ReplicaInfoJson implements ReplicaInfo {
        @JsonProperty("ReplicaType")
        private ReplicaType replicaType;
        @JsonProperty("Replica")
        private ODataId replica;

        @Override
        public ReplicaType getReplicaType() {
            return replicaType;
        }

        @Override
        public ODataId getReplica() {
            return replica;
        }
    }

    public static class CapacitySourceJson implements CapacitySource {
        @JsonProperty("ProvidingPools")
        private Set<ODataId> providingPools;

        @Override
        public Set<ODataId> getProvidingPools() {
            return providingPools;
        }
    }
}
