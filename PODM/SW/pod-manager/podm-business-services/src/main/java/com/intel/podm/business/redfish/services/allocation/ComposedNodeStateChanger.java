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

package com.intel.podm.business.redfish.services.allocation;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.common.enterprise.utils.retry.NumberOfRetriesOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;
import com.intel.podm.common.types.ComposedNodeState;
import com.intel.podm.common.types.Id;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
@Interceptors(RetryOnRollbackInterceptor.class)
public class ComposedNodeStateChanger {
    @Inject
    private GenericDao genericDao;

    @NumberOfRetriesOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void change(Id nodeId, ComposedNodeState nodeState) {
        ComposedNode composedNode = genericDao.find(ComposedNode.class, nodeId);
        composedNode.setComposedNodeState(nodeState);
    }
}
