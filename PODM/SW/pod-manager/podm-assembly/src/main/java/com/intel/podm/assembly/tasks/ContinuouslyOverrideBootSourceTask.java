/*
 * Copyright (c) 2016 Intel Corporation
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

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.BootSourceOverrideInvoker;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.components.ComposedNode;
import com.intel.podm.common.types.BootSourceType;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static com.intel.podm.common.types.BootSourceState.CONTINUOUS;
import static com.intel.podm.common.types.BootSourceType.HDD;
import static com.intel.podm.common.types.BootSourceType.PXE;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

@Dependent
public class ContinuouslyOverrideBootSourceTask extends NodeAssemblyTask {
    @Inject
    private BootSourceOverrideInvoker bootSourceOverrideInvoker;

    @Inject
    private GenericDao genericDao;

    @Override
    @Transactional(REQUIRED)
    public void run() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        ComputerSystem computerSystem = getComputerSystemFromNode(composedNode);
        BootSourceType bootSourceType = isEmpty(composedNode.getRemoteDrives()) ? HDD : PXE;
        try {
            bootSourceOverrideInvoker.overrideBootSource(computerSystem, bootSourceType, CONTINUOUS);
        } catch (ActionException e) {
            throw new RuntimeException("Assembly failed on Computer system action", e);
        }
    }
}
