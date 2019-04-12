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

package com.intel.rsd.nodecomposer.composition.allocation.strategy;

import com.intel.rsd.nodecomposer.business.ODataIdResolvingException;
import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.redfish.EntityTreeTraverser;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.composition.assembly.tasks.NodeTask;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import com.intel.rsd.nodecomposer.persistence.redfish.Zone;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toSet;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class EndpointAllocationStrategy extends ResourceAllocationStrategy {
    private final EntityTreeTraverser traverser;

    private Endpoint endpointToAllocate;
    private Set<Volume> volumesToAllocate = new HashSet<>();

    @Autowired
    public EndpointAllocationStrategy(EntityTreeTraverser traverser) {
        this.traverser = traverser;
    }

    @Override
    public Violations validate() {
        Violations violations = new Violations();
        findResourcesAvailableForAllocation(violations);
        if (violations.hasViolations()) {
            return violations;
        }

        validateAllocationResources(violations);
        return violations;
    }

    @Override
    public List<NodeTask> allocate(ComposedNode composedNode) {
        composedNode.attachAsset(endpointToAllocate);
        volumesToAllocate.forEach(composedNode::attachAsset);

        return emptyList();
    }

    private void findResourcesAvailableForAllocation(Violations violations) {
        ODataId endpointODataId = getODataId();
        try {
            endpointToAllocate = (Endpoint) traverser.traverseDiscoverableEntity(endpointODataId);
            Set<Volume> notAllocatedVolumesFromEndpoint = getNotAllocatedVolumesFromEndpoint(endpointToAllocate);
            volumesToAllocate.addAll(notAllocatedVolumesFromEndpoint);
        } catch (ODataIdResolvingException e) {
            violations.addViolation(format("Specified Endpoint: %s does not exist.", endpointODataId));
        }
    }

    private Set<Volume> getNotAllocatedVolumesFromEndpoint(Endpoint endpoint) {
        return endpoint.getVolumes().stream()
            .filter(vol -> !vol.isAllocated())
            .collect(toSet());
    }

    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:CyclomaticComplexity", "checkstyle:NPathComplexity"})
    private void validateAllocationResources(Violations violations) {
        final ODataId endpointODataId = endpointToAllocate.getUri();
        if (!endpointToAllocate.isAvailable()) {
            violations.addViolation(format("Specified endpoint '%s' should be enabled and healthy in order to invoke actions on it.", endpointODataId));
        }

        if (!endpointToAllocate.isAchievable()) {
            violations.addViolation(format("Specified endpoint '%s' is currently not achievable. Please verify connection with underlying service.",
                endpointODataId));
        }

        if (endpointToAllocate.isAllocated()) {
            violations.addViolation(format("Specified endpoint '%s' is currently allocated by node '%s'.", endpointODataId,
                endpointToAllocate.getComposedNode().getComposedNodeId()));
        }

        final Zone endpointZone = endpointToAllocate.getZone();
        if (endpointZone != null) {
            ODataId zoneODataId = endpointZone.getUri();
            violations.addViolation(format("Specified endpoint '%s' is currently in use in zone '%s'.", endpointODataId, zoneODataId));
        }

        if (!endpointToAllocate.hasRole(TARGET)) {
            violations.addViolation(format("Specified endpoint (%s) does not have role: %s", endpointODataId, TARGET));
        }

        if (!hasConnectedEntityOfTypeVolume(endpointToAllocate)) {
            violations.addViolation(format("Specified endpoint (%s) does not have any volume.", endpointODataId));
        }

        if (volumesToAllocate.isEmpty()) {
            violations.addViolation(format("All volumes of specified endpoint (%s) are currently in use.", endpointODataId));
        }
    }

    private boolean hasConnectedEntityOfTypeVolume(Endpoint endpoint) {
        return endpoint.getConnectedEntities().stream()
            .anyMatch(connectedEntity -> connectedEntity.getEntityLink() instanceof Volume);
    }
}
