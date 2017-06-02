/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.DeepDiscoveryState.INITIAL;
import static com.intel.podm.common.types.DeepDiscoveryState.RUNNING;
import static com.intel.podm.common.types.DeepDiscoveryState.WAITING_TO_START;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ComputerSystemSanitizer {
    @Inject
    private GenericDao genericDao;

    /**
     * Schedules deep discovery on all computer systems on which deep discovery was already in progress
     */
    @Transactional(REQUIRES_NEW)
    public void sanitizeComputerSystemsUsedForDeepDiscovery() {
        genericDao.findAll(ComputerSystem.class).stream()
                .filter(computerSystem -> computerSystem.getMetadata().isInAnyOfStates(WAITING_TO_START, RUNNING))
                .forEach(computerSystem -> computerSystem.getMetadata().setDeepDiscoveryState(INITIAL));
    }
}
