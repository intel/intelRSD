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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.DeepDiscoveryState;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Table;
import java.util.UUID;

import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.types.DeepDiscoveryState.WAITING_TO_START;
import static java.util.Arrays.stream;
import static javax.persistence.EnumType.STRING;

@javax.persistence.Entity
@Table(name = "computer_system_metadata")
public class ComputerSystemMetadata extends Entity {
    @Column(name = "allocated")
    private boolean allocated;

    @Column(name = "deep_discovery_state")
    @Enumerated(STRING)
    private DeepDiscoveryState deepDiscoveryState;

    @Column(name = "task_uuid")
    private UUID taskUuid;

    public boolean isAllocated() {
        return allocated;
    }

    public void setAllocated(boolean allocated) {
        this.allocated = allocated;
    }

    public DeepDiscoveryState getDeepDiscoveryState() {
        return deepDiscoveryState;
    }

    public void setDeepDiscoveryState(DeepDiscoveryState deepDiscoveryState) {
        this.deepDiscoveryState = deepDiscoveryState;
    }

    public UUID getTaskUuid() {
        return taskUuid;
    }

    public void setTaskUuid(UUID taskUuid) {
        this.taskUuid = taskUuid;
    }

    public boolean isInAnyOfStates(DeepDiscoveryState... states) {
        DeepDiscoveryState actualDeepDiscoveryState = getDeepDiscoveryState();
        return actualDeepDiscoveryState != null
            && stream(states).anyMatch(expectedDeepDiscoveryState -> actualDeepDiscoveryState == expectedDeepDiscoveryState);
    }

    public boolean isBeingDeepDiscovered() {
        return isInAnyOfStates(WAITING_TO_START, RUNNING);
    }

    @Override
    public void preRemove() {
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
