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

package com.intel.podm.assembly.tasks;

import com.intel.podm.allocation.ComposedNodeStateChanger;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.common.logger.Logger;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static com.intel.podm.common.types.ComposedNodeState.ASSEMBLED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class SetComposedNodeStateToAssembledTask extends NodeAssemblyTask {

    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private ComposedNodeStateChanger composedNodeStateChanger;


    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @SuppressWarnings({"checkstyle:IllegalCatch"})
    public void run() {
        try {
            composedNodeStateChanger.change(nodeId, ASSEMBLED);
        } catch (RuntimeException e) {
            logger.e("Error while changing node state", e);
        }
    }

    @Override
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        return getComputerSystemFromNode(composedNode).getService().getUuid();
    }
}
