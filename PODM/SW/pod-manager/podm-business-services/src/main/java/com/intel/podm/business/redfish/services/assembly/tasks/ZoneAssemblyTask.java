/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.BusinessApiException;
import com.intel.podm.business.EntityOperationException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.redfish.services.actions.FabricActionsInvoker;
import com.intel.podm.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.podm.common.enterprise.utils.logger.TimeMeasured;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.actions.ZoneActionRequest;
import com.intel.podm.discovery.external.partial.EndpointObtainer;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.utils.IterableHelper.any;
import static java.lang.String.format;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Dependent
public class ZoneAssemblyTask extends NodeTask {
    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    private EndpointObtainer endpointObtainer;

    @Inject
    private FabricActionsInvoker fabricActionService;

    @Inject
    private ZoneActionsInvoker invoker;

    private Endpoint initiatorEndpoint;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        ComposedNode composedNode = getComposedNode();
        initiatorEndpoint = endpointObtainer.getInitiatorEndpoint(composedNode.getComputerSystem(), any(getComposedNode().getEndpoints()).getFabric());

        if (initiatorEndpoint == null) {
            throw new IllegalStateException(format("There is no initiator endpoint on ComputerSystem with UUID: %s",
                composedNode.getComputerSystem().getUuid()));
        }

        if (initiatorEndpoint.getZone() == null) {
            createZone(composedNode);
        } else {
            updateZone(composedNode);
        }
    }

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public UUID getServiceUuid() {
        return any(getComposedNode().getEndpoints()).getService().getUuid();
    }

    private ComposedNode getComposedNode() {
        return genericDao.find(ComposedNode.class, nodeId);
    }

    private ZoneActionRequest buildCreationRequest(Set<Endpoint> targetEndpoints) {
        targetEndpoints.add(initiatorEndpoint);

        return new ZoneActionRequest(targetEndpoints.stream().map(DiscoverableEntity::getSourceUri).collect(toSet()));
    }

    private void updateZone(ComposedNode composedNode) {
        try {
            invoker.updateZone(initiatorEndpoint.getZone(), composedNode.getEndpoints());
        } catch (BusinessApiException e) {
            logger.e("Zone update failed for ComposedNode: {}, details: {}", nodeId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private void createZone(ComposedNode composedNode) {
        try {
            fabricActionService.createZone(initiatorEndpoint.getFabric(), buildCreationRequest(composedNode.getEndpoints()));
        } catch (EntityOperationException e) {
            logger.e("Zone creation failed for ComposedNode: {}, details: {}", nodeId, e.getMessage());
            throw new RuntimeException(e);
        }
    }
}
