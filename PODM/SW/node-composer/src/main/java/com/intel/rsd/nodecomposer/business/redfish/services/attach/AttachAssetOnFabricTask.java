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

package com.intel.rsd.nodecomposer.business.redfish.services.attach;

import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContext;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.InitiatorEndpointFinder;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.transactions.RequiresNewTransactionWrapper;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class AttachAssetOnFabricTask implements RequiresNewTransactionWrapper.Task<AttachableContext, BusinessApiException> {
    private final ZoneActionsInvoker zoneActionsInvoker;
    private final InitiatorEndpointFinder initiatorEndpointFinder;
    private final ZoneObtainer zoneObtainer;
    private final EntityTreeTraverser traverser;

    private AttachableContext context;
    private Attacher attachStrategy;

    @Autowired
    public AttachAssetOnFabricTask(ZoneActionsInvoker zoneActionsInvoker, InitiatorEndpointFinder initiatorEndpointFinder,
                                   EntityTreeTraverser entityTreeTraverser, ZoneObtainer zoneObtainer) {
        this.zoneActionsInvoker = zoneActionsInvoker;
        this.initiatorEndpointFinder = initiatorEndpointFinder;
        this.zoneObtainer = zoneObtainer;
        this.traverser = entityTreeTraverser;
    }

    @Override
    public AttachableContext execute() throws BusinessApiException {
        val composedNode = traverser.traverseComposedNode(context.getNodeODataId());
        val assetToAttach = traverser.tryGetDiscoverableEntity(context.getAssetODataId());
        configureFabric(composedNode, assetToAttach);
        return context;
    }

    public AttachAssetOnFabricTask init(Attacher attachStrategy, AttachableContext context) {
        this.attachStrategy = attachStrategy;
        this.context = context;
        return this;
    }

    private void configureFabric(ComposedNode composedNode, DiscoverableEntity assetToAttach) throws BusinessApiException {
        context.setInitiatorEndpointODataId(getOrCreateInitiatorEndpoint(composedNode));
        context.setTargetEndpointODataId(
            attachStrategy.getOrCreateTargetEndpoint(assetToAttach, context.getFabricODataId(), context.getFabricType()));
        context.setRelatedZoneODataId(getOrCreateZone());
    }

    private ODataId getOrCreateZone() throws BusinessApiException {
        val zone = zoneObtainer.getZoneFromEndpoint(context.getInitiatorEndpointODataId());
        if (zone.isPresent()) {
            return zoneActionsInvoker.updateZone(zone.get().getUri(), context.getTargetEndpointODataId());
        } else {
            return zoneActionsInvoker.createZone(context.getFabricODataId(), context.getInitiatorEndpointODataId(),
                context.getTargetEndpointODataId());
        }
    }

    private ODataId getOrCreateInitiatorEndpoint(ComposedNode composedNode) {
        return initiatorEndpointFinder.findOrCreateInitiatorEndpoint(composedNode, context.getFabricODataId(),
            context.getFabricType());
    }
}
