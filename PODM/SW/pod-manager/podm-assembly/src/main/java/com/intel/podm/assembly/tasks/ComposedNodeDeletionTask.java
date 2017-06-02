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

import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

public class ComposedNodeDeletionTask extends NodeAssemblyTask {

    @Inject
    private GenericDao genericDao;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Override
    @NumberOfRetriesOnRollback(3)
    @Interceptors(RetryOnRollbackInterceptor.class)
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        genericDao.remove(composedNode);
    }

    @Override
    public UUID getServiceUuid() {
        //todo: current implementation synchronizes composed node deletion across PODM instance, make sure that this is a best solution
        return serviceConfigHolder.get().getUuid();
    }
}
