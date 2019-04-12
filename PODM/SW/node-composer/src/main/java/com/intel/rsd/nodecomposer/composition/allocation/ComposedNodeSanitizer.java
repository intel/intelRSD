/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation;

import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;

import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ALLOCATING;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.ASSEMBLING;
import static com.intel.rsd.nodecomposer.types.ComposedNodeState.FAILED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Slf4j
@Component
@Scope(SCOPE_SINGLETON)
public class ComposedNodeSanitizer {
    private final GenericDao genericDao;

    @Autowired
    public ComposedNodeSanitizer(GenericDao genericDao) {
        this.genericDao = genericDao;
    }

    @Transactional(REQUIRES_NEW)
    public void sanitizeComputerSystemsUsedInComposedNodes() {
        genericDao.findAll(ComposedNode.class).stream()
            //TODO: pzak - what about these states in Hazelcast World?
            .filter(composedNode -> composedNode.isInAnyOfStates(ALLOCATING, /*ALLOCATED,*/ ASSEMBLING))
            .forEach(composedNode -> {
                log.debug("Failing not assembled Composed Node [" + composedNode.getComposedNodeId() + "]");
                composedNode.setComposedNodeState(FAILED);
            });
    }
}
