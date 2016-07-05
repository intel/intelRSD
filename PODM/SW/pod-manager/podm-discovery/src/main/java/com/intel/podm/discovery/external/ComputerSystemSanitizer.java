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

package com.intel.podm.discovery.external;

import com.intel.podm.business.entities.dao.ComputerSystemDao;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.INITIAL;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.RUNNING;
import static com.intel.podm.business.entities.redfish.base.DeepDiscoverable.DeepDiscoveryState.WAITING_TO_START;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ComputerSystemSanitizer {
    @Inject
    ComputerSystemDao computerSystemDao;

    /**
     * Schedules deep discovery on all computer systems on which deep discovery was already in progress
     */
    @Transactional(REQUIRES_NEW)
    public void sanitizeComputerSystemsUsedForDeepDiscovery() {
        computerSystemDao.findAll().stream()
                .filter(computerSystem -> computerSystem.isInAnyOfStates(WAITING_TO_START, RUNNING))
                .forEach(computerSystem -> computerSystem.setDeepDiscoveryState(INITIAL));
    }
}
