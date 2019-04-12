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
import com.intel.rsd.nodecomposer.composition.assembly.tasks.TargetEndpointAssemblyTaskFactory;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Volume;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.List;

import static com.intel.rsd.nodecomposer.types.EntityRole.TARGET;
import static java.lang.String.format;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.Objects.isNull;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class VolumeAllocationStrategy extends ResourceAllocationStrategy {
    private final EntityTreeTraverser traverser;

    private Volume volumeToAllocate;
    private Endpoint endpointToAllocate;
    private TargetEndpointAssemblyTaskFactory targetEndpointAssemblyTaskFactory;

    @Autowired
    public VolumeAllocationStrategy(EntityTreeTraverser traverser, TargetEndpointAssemblyTaskFactory targetEndpointAssemblyTaskFactory) {
        this.traverser = traverser;
        this.targetEndpointAssemblyTaskFactory = targetEndpointAssemblyTaskFactory;
    }

    @Override
    public Violations validate() {
        Violations violations = new Violations();
        verifyExistenceOfRequestedVolume(violations);
        verifyIfAvailableEndpointExistForVolumeOrItCanBeCreated(violations);
        if (violations.hasViolations()) {
            return violations;
        }

        validateVolume(violations);
        return violations;
    }

    @Override
    public List<NodeTask> allocate(ComposedNode composedNode) {
        composedNode.attachAsset(volumeToAllocate);

        if (endpointToAllocate != null) {
            composedNode.addEndpoint(endpointToAllocate);
            return emptyList();
        } else {
            return singletonList(targetEndpointAssemblyTaskFactory.newTargetEndpointAssemblyTask(composedNode.getUri(), volumeToAllocate.getUri()));
        }
    }

    private void verifyExistenceOfRequestedVolume(Violations violations) {
        ODataId volumeODataId = getODataId();
        try {
            volumeToAllocate = (Volume) traverser.traverseDiscoverableEntity(volumeODataId);
        } catch (ODataIdResolvingException e) {
            violations.addViolation(format("Specified Volume: %s does not exist.", volumeODataId));
        }
    }

    private void verifyIfAvailableEndpointExistForVolumeOrItCanBeCreated(Violations violations) {
        if (!volumeToAllocate.getEndpoints().isEmpty()) {
            endpointToAllocate = volumeToAllocate.getEndpoints().stream()
                .filter(endpoint -> !endpoint.isAllocated())
                .filter(endpoint -> isNull(endpoint.getZone()))
                .filter(endpoint -> endpoint.hasRole(TARGET))
                .findFirst()
                .orElseGet(() -> {
                    violations.addViolation("All endpoints attached to volume " + volumeToAllocate + " are used in zone.");
                    return null;
                });
        }
    }

    private void validateVolume(Violations violations) {
        ODataId volumeODataId = volumeToAllocate.getUri();
        if (!volumeToAllocate.isAvailable()) {
            violations.addViolation(format("Specified volume (%s) should be enabled and healthy in order to invoke actions on it.", volumeODataId));
        }

        if (!volumeToAllocate.isAchievable()) {
            violations.addViolation(format("Specified volume (%s) is currently not achievable. Please verify connection with underlying StorageService.",
                volumeODataId));
        }

        if (volumeToAllocate.isAllocated()) {
            violations.addViolation(format("Specified volume (%s) is currently in use.", volumeODataId));
        }
    }
}
