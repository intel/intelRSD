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
import com.intel.rsd.nodecomposer.business.EntityOperationException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.EndpointUpdateInvoker;
import com.intel.rsd.nodecomposer.persistence.dao.EndpointDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.types.Chap;
import com.intel.rsd.nodecomposer.types.actions.EndpointUpdateDefinition;
import com.intel.rsd.nodecomposer.types.actions.EndpointUpdateDefinition.Authentication;
import com.intel.rsd.nodecomposer.utils.measures.TimeMeasured;
import lombok.Setter;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.io.Serializable;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static com.intel.rsd.nodecomposer.types.Protocol.ISCSI;
import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@SpringAware
@Component
@Scope(SCOPE_PROTOTYPE)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class PatchEndpointsAssemblyTask extends NodeTask implements Serializable {
    private static final long serialVersionUID = 4712065488868876982L;

    @Autowired
    private transient GenericDao genericDao;

    @Autowired
    private transient EndpointDao endpointDao;

    @Autowired
    private transient EndpointUpdateInvoker endpointUpdateInvoker;

    @Setter
    private Chap chap;

    @Override
    @TimeMeasured(tag = "[AssemblyTask]")
    @Transactional(REQUIRES_NEW)
    public void run() {
        val node = genericDao.find(ComposedNode.class, composedNodeODataId);
        val system = getComputerSystemFromNode(node);

        val targetEndpoints = findIscsiTargetEndpoints(node);
        requires(!targetEndpoints.isEmpty(), format("Cannot find any target endpoint related with node(%s)", composedNodeODataId));

        val fabric = retrieveFabric(targetEndpoints);
        requiresNonNull(fabric, format("Cannot determine fabric related with Node's(%s) target endpoints", composedNodeODataId));

        val initiatorEndpoint = endpointDao.findInitiatorEndpointBySystemAndFabric(system.getUri(), fabric.getUri());
        requiresNonNull(initiatorEndpoint, format("Cannot find initiator endpoint for system(%s) and fabric(%s)", system.getUri(), fabric.getUri()));

        try {
            updateInitiatorEndpoint(initiatorEndpoint);
            updateTargetEndpoints(targetEndpoints);
        } catch (EntityOperationException e) {
            throw new RuntimeException("Endpoint update failed: " + e.getMessage(), e);
        }
    }

    private Fabric retrieveFabric(Set<Endpoint> targetEndpoints) {
        return targetEndpoints.stream()
            .map(Endpoint::getFabric)
            .findFirst()
            .orElseThrow(() -> new RuntimeException("Unable to retrieve Fabric from ISCSI endpoints"));
    }

    private void updateInitiatorEndpoint(Endpoint endpoint) throws EntityOperationException {
        EndpointUpdateDefinition initiatorDefinition = new EndpointUpdateDefinition();
        Authentication authentication = initiatorDefinition.getOem().getRackScaleOem().getAuthentication();

        authentication.setUsername(chap.getUsername().get());
        authentication.setPassword(chap.getSecret().get());

        endpointUpdateInvoker.updateEndpoint(endpoint, initiatorDefinition);
    }

    private void updateTargetEndpoints(Set<Endpoint> endpoints) throws EntityOperationException {
        EndpointUpdateDefinition targetDefinition = new EndpointUpdateDefinition();
        Authentication authentication = targetDefinition.getOem().getRackScaleOem().getAuthentication();

        authentication.setUsername(chap.getMutualUsername().get());
        authentication.setPassword(chap.getMutualSecret().get());

        for (Endpoint endpoint : endpoints) {
            endpointUpdateInvoker.updateEndpoint(endpoint, targetDefinition);
        }
    }

    private Set<Endpoint> findIscsiTargetEndpoints(ComposedNode composedNode) {
        return composedNode.getEndpoints().stream()
            .filter(endpoint -> ISCSI.equals(endpoint.getProtocol()))
            .filter(byTargetRole())
            .collect(toSet());
    }

    private Predicate<Endpoint> byTargetRole() {
        return endpoint -> endpoint.getConnectedEntities().stream()
            .anyMatch(connectedEntity -> TARGET.equals(connectedEntity.getEntityRole()));
    }
}
