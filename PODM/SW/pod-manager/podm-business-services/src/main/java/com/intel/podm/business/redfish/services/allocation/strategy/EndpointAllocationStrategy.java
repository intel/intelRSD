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

package com.intel.podm.business.redfish.services.allocation.strategy;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.redfish.ComposedNode;
import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.Volume;
import com.intel.podm.business.entities.redfish.Zone;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.stream.Collectors.toSet;

@Dependent
public class EndpointAllocationStrategy extends ResourceAllocationStrategy {
    @Inject
    private EntityTreeTraverser traverser;

    private Endpoint endpointToAllocate;
    private Set<Volume> volumesToAllocate = new HashSet<>();

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
        endpointToAllocate.getMetadata().setAllocated(true);
        composedNode.addEndpoint(endpointToAllocate);
        composedNode.setAssociatedStorageServiceUuid(endpointToAllocate.getService().getUuid());
        composedNode.addAssociatedEndpointIdentifiers(endpointToAllocate.getIdentifiers());

        volumesToAllocate.forEach(volume -> {
            volume.getMetadata().setAllocated(true);
            composedNode.addVolume(volume);
        });

        return emptyList();
    }

    @Override
    public Violations findResources() {
        return new Violations();
    }

    private void findResourcesAvailableForAllocation(Violations violations) {
        Context endpointContext = getContext();
        try {
            endpointToAllocate = (Endpoint) traverser.traverse(endpointContext);
            Set<Volume> notAllocatedVolumesFromEndpoint = getNotAllocatedVolumesFromEndpoint(endpointToAllocate);
            volumesToAllocate.addAll(notAllocatedVolumesFromEndpoint);
        } catch (ContextResolvingException e) {
            violations.addViolation(format("Specified %s: %s does not exist.", endpointContext.getType(), endpointContext));
        }
    }

    private Set<Volume> getNotAllocatedVolumesFromEndpoint(Endpoint endpoint) {
        return endpoint.getVolumes().stream()
            .filter(vol -> !vol.getMetadata().isAllocated())
            .collect(toSet());
    }

    @SuppressWarnings({"checkstyle:MethodLength", "checkstyle:CyclomaticComplexity", "checkstyle:NPathComplexity"})
    private void validateAllocationResources(Violations violations) {
        final Context endpointContext = toContext(endpointToAllocate);
        if (!endpointToAllocate.isAvailable()) {
            violations.addViolation(format("Specified endpoint '%s' should be enabled and healthy in order to invoke actions on it.", endpointContext));
        }

        if (endpointToAllocate.getMetadata().isAllocated()) {
            violations.addViolation(format("Specified endpoint '%s' is currently allocated by node '%s'.", endpointContext,
                endpointToAllocate.getComposedNode().getId()));
        }

        final Zone endpointZone = endpointToAllocate.getZone();
        if (endpointZone != null) {
            Context zoneContext = toContext(endpointZone);
            violations.addViolation(format("Specified endpoint '%s' is currently in use in zone '%s'.", endpointContext, zoneContext));
        }

        if (!endpointToAllocate.hasRole(TARGET)) {
            violations.addViolation(format("Specified endpoint (%s) does not have role: %s", endpointContext, TARGET));
        }

        if (!hasConnectedEntityOfTypeVolume(endpointToAllocate)) {
            violations.addViolation(format("Specified endpoint (%s) does not have any volume.", endpointContext));
        }

        if (volumesToAllocate.isEmpty()) {
            violations.addViolation(format("All volumes of specified endpoint (%s) are currently in use.", endpointContext));
        }
    }

    private boolean hasConnectedEntityOfTypeVolume(Endpoint endpoint) {
        return endpoint.getConnectedEntities().stream()
            .anyMatch(connectedEntity -> connectedEntity.getEntityLink() instanceof Volume);
    }
}
