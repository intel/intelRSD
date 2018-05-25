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

package com.intel.podm.business.redfish;

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.ResourceStateMismatchException;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.ServiceConfig;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.UUID;

import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ServiceTraverser {
    @Inject
    private EntityTreeTraverser traverser;

    @Inject
    @Config
    private Holder<ServiceConfig> serviceConfigHolder;

    @Transactional(REQUIRES_NEW)
    public UUID traverseServiceUuid(Context context) throws BusinessApiException {
        Entity entity = traverser.traverse(context);

        if (DiscoverableEntity.class.isAssignableFrom(entity.getClass())) {
            if (((DiscoverableEntity) entity).getService() != null) {
                return ((DiscoverableEntity) entity).getService().getUuid();
            }
            return serviceConfigHolder.get().getUuid();
        }

        if (ComposedNode.class.isAssignableFrom(entity.getClass())) {
            return getComputerSystemServiceUuid(context);
        }

        throw new ContextResolvingException(context);
    }

    private UUID getComputerSystemServiceUuid(Context composedNodeContext) throws BusinessApiException {
        ComposedNode composedNode = (ComposedNode) traverser.traverse(composedNodeContext);
        ComputerSystem computerSystem = composedNode.getComputerSystem();
        if (computerSystem != null) {
            return computerSystem.getService().getUuid();
        }
        throw new ResourceStateMismatchException("Requested Composed Node has detached Computer System.");
    }
}
