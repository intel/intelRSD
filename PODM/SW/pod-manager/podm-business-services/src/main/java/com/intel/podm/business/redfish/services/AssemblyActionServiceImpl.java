/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.assembly.AssemblyException;
import com.intel.podm.business.redfish.services.assembly.NodeAssembler;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.ActionService;
import com.intel.podm.business.services.redfish.requests.AssemblyRequest;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollback;
import com.intel.podm.common.enterprise.utils.retry.RetryOnRollbackInterceptor;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.interceptor.Interceptors;
import javax.transaction.Transactional;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@RequestScoped
@Interceptors(RetryOnRollbackInterceptor.class)
class AssemblyActionServiceImpl implements ActionService<AssemblyRequest> {
    @Inject
    private NodeAssembler nodeAssembler;

    @Inject
    private EntityTreeTraverser traverser;

    @Override
    @RetryOnRollback(3)
    @Transactional(REQUIRES_NEW)
    public void perform(Context target, AssemblyRequest request) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(target);
        try {
            nodeAssembler.assemble(composedNode);
        } catch (AssemblyException e) {
            throw new EntityOperationException("Assembly failed: " + e.getMessage(), e);
        }
    }
}
