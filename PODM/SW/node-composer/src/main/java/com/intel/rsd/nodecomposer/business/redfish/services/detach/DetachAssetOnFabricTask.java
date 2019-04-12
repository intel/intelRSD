/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContext;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.utils.transactions.RequiresNewTransactionWrapper;
import lombok.SneakyThrows;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.Optional;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class DetachAssetOnFabricTask implements RequiresNewTransactionWrapper.Task<AttachableContext, RuntimeException> {
    private final ZoneActionsInvoker zoneActionsInvoker;
    private final EntityTreeTraverser traverser;

    private AttachableContext context;
    private Detacher detachStrategy;

    @Autowired
    public DetachAssetOnFabricTask(ZoneActionsInvoker zoneActionsInvoker, EntityTreeTraverser entityTreeTraverser) {
        this.zoneActionsInvoker = zoneActionsInvoker;
        this.traverser = entityTreeTraverser;
    }

    @Override
    @SneakyThrows
    public AttachableContext execute() {
        val composedNode = traverser.traverseComposedNode(context.getNodeODataId());
        val assetToDetach = traverser.tryGetDiscoverableEntity(context.getAssetODataId());

        Optional<Endpoint> targetEndpoint = detachStrategy.retrieveTargetEndpoint((AttachableAsset) assetToDetach);
        targetEndpoint.ifPresent(endpoint -> {
                context.setTargetEndpointODataId(endpoint.getUri());
                context.setRelatedZoneODataId(endpoint.getZone().getUri());
                zoneActionsInvoker.removeFromZone(context.getTargetEndpointODataId(), context.getRelatedZoneODataId());
                detachStrategy.removeResource(context.getTargetEndpointODataId());
            }
        );
        detachStrategy.performResourceSpecificActions(composedNode, assetToDetach);
        return context;
    }

    public DetachAssetOnFabricTask init(Detacher detacher, AttachableContext context) {
        this.detachStrategy = detacher;
        this.context = context;
        return this;
    }
}
