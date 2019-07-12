/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly.tasks;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.FabricActionsInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.ZoneActionsInvoker;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import com.intel.rsd.nodecomposer.types.actions.ZoneActionRequest;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.HashSet;
import java.util.Set;

import static java.lang.String.format;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Slf4j
@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ZoneAssemblyTask extends NodeTask {
    private static final long serialVersionUID = 4716521385997852306L;

    @Autowired
    private transient GenericDao genericDao;

    @Autowired
    private transient ZoneActionsInvoker invoker;

    @Autowired
    private transient EndpointDao endpointDao;

    @Autowired
    private transient FabricActionsInvoker fabricActionService;

    @Setter
    private ODataId fabricOdataId;

    @Override
    @Transactional(REQUIRES_NEW)
    @TimeMeasured(tag = "[AssemblyTask]")
    public void run() {
        val node = getComposedNode();
        val system = getComputerSystemFromNode(node);
        val initiatorEndpoint = endpointDao.findInitiatorEndpointBySystemAndFabric(system.getUri(), fabricOdataId);

        if (initiatorEndpoint == null) {
            throw new IllegalStateException(format("There is no initiator endpoint on ComputerSystem with UUID: %s", system.getUuid()));
        }

        if (initiatorEndpoint.getZone() == null) {
            createZone(node, initiatorEndpoint);
        } else {
            HashSet<Endpoint> endpoints = new HashSet<>(node.getEndpoints());
            endpoints.add(initiatorEndpoint);
            updateZone(initiatorEndpoint.getZone(), endpoints);
        }
    }

    private ComposedNode getComposedNode() {
        return genericDao.find(ComposedNode.class, composedNodeODataId);
    }

    private ZoneActionRequest buildCreationRequest(Endpoint initiatorEndpoint, Set<Endpoint> targetEndpoints) {
        targetEndpoints.add(initiatorEndpoint);

        return new ZoneActionRequest(targetEndpoints.stream().map(endpoint -> endpoint.getUri().toUri()).collect(toSet()));
    }

    private void updateZone(Zone zone, Set<Endpoint> relatedEndpoints) {
        try {
            invoker.updateZone(zone, relatedEndpoints);
        } catch (BusinessApiException e) {
            log.error("Zone update failed for ComposedNode: {}, details: {}", composedNodeODataId, e.getMessage());
            throw new RuntimeException(e);
        }
    }

    private void createZone(ComposedNode composedNode, Endpoint initiatorEndpoint) {
        try {
            fabricActionService.createZone(initiatorEndpoint.getFabric(), buildCreationRequest(initiatorEndpoint, composedNode.getEndpoints()));
        } catch (EntityOperationException e) {
            log.error("Zone creation failed for ComposedNode: {}, details: {}", composedNodeODataId, e.getMessage());
            throw new RuntimeException(e);
        }
    }
}
