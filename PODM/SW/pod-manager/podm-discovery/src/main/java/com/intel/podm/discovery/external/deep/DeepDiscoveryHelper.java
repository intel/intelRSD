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

package com.intel.podm.discovery.external.deep;

import com.intel.podm.business.entities.redfish.ComputerSystem;

import javax.enterprise.context.Dependent;

import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.RUNNING;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.SCHEDULED_MANUALLY;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.WAITING_TO_START;
import static com.intel.podm.common.types.DiscoveryState.DEEP_IN_PROGRESS;

@Dependent
public class DeepDiscoveryHelper {
    public void triggerDeepDiscovery(ComputerSystem computerSystem) {
        computerSystem.setDiscoveryState(DEEP_IN_PROGRESS);
        computerSystem.setAllocated(true);
        computerSystem.setDeepDiscoveryState(SCHEDULED_MANUALLY);
    }

    public boolean isDeepDiscoveryInProgress(ComputerSystem computerSystem) {
        return computerSystem.isInAnyOfStates(SCHEDULED_MANUALLY, WAITING_TO_START, RUNNING);
    }
}
