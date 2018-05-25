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
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.assembly.tasks.EndpointTaskFactory;
import com.intel.podm.business.redfish.services.assembly.tasks.NodeTask;
import com.intel.podm.business.services.context.Context;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.List;

import static com.intel.podm.business.redfish.Contexts.toContext;
import static com.intel.podm.common.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.Objects.isNull;

@Dependent
public class VolumeAllocationStrategy extends ResourceAllocationStrategy {
    @Inject
    private EndpointTaskFactory endpointTaskFactory;

    @Inject
    private EntityTreeTraverser traverser;

    private Volume volumeToAllocate;
    private Endpoint endpointToAllocate;

    @Override
    public Violations validate() {
        Violations violations = new Violations();
        verifyExistenceOfRequestedVolume(violations);
        verifyIfAvailableEndpointExistForVolumeOrItCanBeCreated(violations);
        return violations;
    }

    @Override
    public List<NodeTask> allocate(ComposedNode composedNode) {
        volumeToAllocate.getMetadata().setAllocated(true);
        composedNode.addVolume(volumeToAllocate);
        composedNode.setAssociatedStorageServiceUuid(volumeToAllocate.getService().getUuid());
        composedNode.addAssociatedVolumeIdentifiers(volumeToAllocate.getIdentifiers());

        if (endpointToAllocate != null) {
            endpointToAllocate.getMetadata().setAllocated(true);
            composedNode.addEndpoint(endpointToAllocate);
            return emptyList();
        } else {
            return singletonList(endpointTaskFactory.create(volumeToAllocate.getId(), TARGET, toContext(volumeToAllocate)));
        }
    }

    @Override
    public Violations findResources() {
        Context volumeContext = toContext(volumeToAllocate);
        Violations violations = new Violations();
        if (!volumeToAllocate.isAvailable()) {
            violations.addViolation(format("Specified volume (%s) should be enabled and healthy in order to invoke actions on it.", volumeContext));
        }
        if (volumeToAllocate.getMetadata().isAllocated()) {
            violations.addViolation(format("Specified volume (%s) is currently in use.", volumeContext));
        }
        return violations;
    }

    private void verifyExistenceOfRequestedVolume(Violations violations) {
        Context volumeContext = getContext();
        try {
            volumeToAllocate = (Volume) traverser.traverse(volumeContext);
        } catch (ContextResolvingException e) {
            violations.addViolation(format("Specified %s: %s does not exist.", volumeContext.getType(), volumeContext));
        }
    }

    private void verifyIfAvailableEndpointExistForVolumeOrItCanBeCreated(Violations violations) {
        if (!volumeToAllocate.getEndpoints().isEmpty()) {
            endpointToAllocate = volumeToAllocate.getEndpoints().stream()
                .filter(endpoint -> !endpoint.getMetadata().isAllocated())
                .filter(endpoint -> isNull(endpoint.getZone()))
                .filter(endpoint -> endpoint.hasRole(TARGET))
                .findFirst()
                .orElseGet(() -> {
                    violations.addViolation("All endpoints attached to volume " + toContext(volumeToAllocate) + " are used in zone.");
                    return null;
                });
        }
    }
}
