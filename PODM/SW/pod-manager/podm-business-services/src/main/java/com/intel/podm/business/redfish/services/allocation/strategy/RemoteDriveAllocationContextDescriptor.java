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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.Violations;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.ReplicaType;

import java.math.BigDecimal;
import java.net.URI;

@SuppressWarnings({"checkstyle:MethodCount"})
public class RemoteDriveAllocationContextDescriptor {
    private Id storagePoolId;
    private URI storagePoolUri;
    private Id storageServiceId;
    private URI masterUri;
    private Violations violations;
    private BigDecimal capacity;
    private Protocol protocol;
    private ReplicaType replicaType;
    private URI newRemoteVolumeUri;

    public Id getStoragePoolId() {
        return storagePoolId;
    }

    public void setStoragePoolId(Id storagePoolId) {
        this.storagePoolId = storagePoolId;
    }

    public URI getStoragePoolUri() {
        return storagePoolUri;
    }

    public void setStoragePoolUri(URI storagePoolUri) {
        this.storagePoolUri = storagePoolUri;
    }

    public Id getStorageServiceId() {
        return storageServiceId;
    }

    public void setStorageServiceId(Id storageServiceId) {
        this.storageServiceId = storageServiceId;
    }

    public URI getMasterUri() {
        return masterUri;
    }

    public void setMasterUri(URI masterUri) {
        this.masterUri = masterUri;
    }

    public Violations getViolations() {
        return violations;
    }

    public void setViolations(Violations violations) {
        this.violations = violations;
    }

    public BigDecimal getCapacity() {
        return capacity;
    }

    public void setCapacity(BigDecimal capacity) {
        this.capacity = capacity;
    }

    public Protocol getProtocol() {
        return protocol;
    }

    public void setProtocol(Protocol protocol) {
        this.protocol = protocol;
    }

    public ReplicaType getReplicaType() {
        return replicaType;
    }

    public void setReplicaType(ReplicaType replicaType) {
        this.replicaType = replicaType;
    }

    public URI getNewRemoteVolumeUri() {
        return newRemoteVolumeUri;
    }

    public void setNewRemoteVolumeUri(URI newRemoteVolumeUri) {
        this.newRemoteVolumeUri = newRemoteVolumeUri;
    }
}
