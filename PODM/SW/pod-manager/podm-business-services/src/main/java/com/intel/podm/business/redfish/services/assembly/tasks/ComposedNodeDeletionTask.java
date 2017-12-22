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

package com.intel.podm.business.redfish.services.assembly.tasks;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.UUID;

@Dependent
public class ComposedNodeDeletionTask extends NodeRemovalTask {
    @Inject
    private GenericDao genericDao;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Override
    public void disassemble() {
    }

    @Override
    public void deallocate() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        genericDao.remove(composedNode);
    }

    @Override
    public UUID getServiceUuid() {
        return serviceConfigHolder.get().getUuid();
    }
}
