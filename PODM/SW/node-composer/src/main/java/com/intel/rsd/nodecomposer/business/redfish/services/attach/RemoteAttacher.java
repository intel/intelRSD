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
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachDetachOperationContext;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.InitiatorEndpointFinder;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.discovery.external.partial.ZoneObtainer;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.types.Protocol;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class RemoteAttacher {
    private final ZoneActionsInvoker zoneActionsInvoker;
    private final InitiatorEndpointFinder initiatorEndpointFinder;
    private final ZoneObtainer zoneObtainer;
    private final EntityTreeTraverser traverser;

    @Autowired
    public RemoteAttacher(ZoneActionsInvoker zoneActionsInvoker, InitiatorEndpointFinder initiatorEndpointFinder,
                          EntityTreeTraverser entityTreeTraverser, ZoneObtainer zoneObtainer) {
        this.zoneActionsInvoker = zoneActionsInvoker;
        this.initiatorEndpointFinder = initiatorEndpointFinder;
        this.zoneObtainer = zoneObtainer;
        this.traverser = entityTreeTraverser;
    }

    public void execute(Attacher attachStrategy, AttachDetachOperationContext context) throws BusinessApiException {
        val composedNode = traverser.traverseComposedNode(context.getNodeODataId());
        val assetToAttach = traverser.tryGetDiscoverableEntity(context.getAssetODataId());

        val targetEndpointOdataId = attachStrategy.getOrCreateTargetEndpoint(assetToAttach, context.getFabricODataId(), context.getFabricType());
        val initiatorEndpointOdataId = getOrCreateInitiatorEndpoint(composedNode, context.getFabricODataId(), context.getFabricType());
        val zoneOdataId = getOrCreateZone(context.getFabricODataId(), targetEndpointOdataId, initiatorEndpointOdataId);

        context.setInitiatorEndpointODataId(initiatorEndpointOdataId);
        context.setTargetEndpointODataId(targetEndpointOdataId);
        context.setRelatedZoneODataId(zoneOdataId);
    }

    private ODataId getOrCreateZone(ODataId fabricODataId, ODataId targetEndpointOdataId, ODataId initiatorEndpointOdataId) throws BusinessApiException {
        val zone = zoneObtainer.getZoneFromEndpoint(initiatorEndpointOdataId);
        if (zone.isPresent()) {
            return zoneActionsInvoker.updateZone(zone.get().getUri(), targetEndpointOdataId);
        } else {
            return zoneActionsInvoker.createZone(fabricODataId, initiatorEndpointOdataId, targetEndpointOdataId);
        }
    }

    private ODataId getOrCreateInitiatorEndpoint(ComposedNode composedNode, ODataId fabricOdataId, Protocol fabricType) {
        return initiatorEndpointFinder.findOrCreateInitiatorEndpoint(composedNode, fabricOdataId, fabricType);
    }
}
