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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.ComposedNodeState.ALLOCATED;
import static com.intel.podm.common.types.ComposedNodeState.ALLOCATING;
import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLING;
import static com.intel.podm.common.types.ComposedNodeState.FAILED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ComposedNodeSanitizer {
    @Inject
    private GenericDao genericDao;

    @Inject
    private Logger logger;

    @Transactional(REQUIRES_NEW)
    public void sanitizeComputerSystemsUsedInComposedNodes() {
        genericDao.findAll(ComposedNode.class).stream()
                .filter(composedNode -> composedNode.isInAnyOfStates(ALLOCATING, ALLOCATED, ASSEMBLING))
                .forEach(composedNode -> {
                    logger.d("Failing not assembled Composed Node [" + composedNode.getId() + "]");
                    composedNode.setComposedNodeState(FAILED);
                });
    }
}
