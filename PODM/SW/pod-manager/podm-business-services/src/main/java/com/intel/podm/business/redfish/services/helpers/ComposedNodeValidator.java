/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.redfish.services.helpers;

import com.intel.podm.allocation.RequestValidationException;
import com.intel.podm.allocation.validation.Violations;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.dto.redfish.ContextPossessor;
import com.intel.podm.business.dto.redfish.RequestedMasterDrive;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.dto.redfish.RequestedRemoteDrive;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;

import javax.inject.Inject;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.util.Collections.emptyList;
import static java.util.Objects.nonNull;
import static java.util.stream.Collectors.toList;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

public class ComposedNodeValidator {
    @Inject
    private DomainObjectTreeTraverser traverser;

    public void validateExistenceOfIncludedResources(RequestedNode requestedNode) throws RequestValidationException {
        Set<Context> contexts = new HashSet<>();
        contexts.addAll(getContexts(requestedNode.getProcessors()));
        contexts.addAll(getContexts(requestedNode.getMemoryModules()));
        contexts.addAll(getContextsOfMasterDrives(requestedNode.getRemoteDrives()));
        contexts.addAll(getContexts(requestedNode.getLocalDrives()));
        contexts.addAll(getContexts(requestedNode.getEthernetInterfaces()));

        validateContexts(contexts);
    }

    private List<Context> getContexts(List<? extends ContextPossessor> contextPossessors) {
        if (isEmpty(contextPossessors)) {
            return emptyList();
        }

        List<Context> contexts = new ArrayList<>();
        contextPossessors.stream().forEach(contextPossessor -> {
            if (contextPossessor.getResourceContext() != null) {
                contexts.add(contextPossessor.getResourceContext());
            }
            if (contextPossessor.getChassisContext() != null) {
                contexts.add(contextPossessor.getChassisContext());
            }
        });
        return contexts;
    }

    private List<Context> getContextsOfMasterDrives(List<RequestedRemoteDrive> remoteDrives) {
        if (isEmpty(remoteDrives)) {
            return emptyList();
        }

        return remoteDrives.stream()
                .filter(remoteDrive -> nonNull(remoteDrive.getMaster()))
                .map(RequestedRemoteDrive::getMaster)
                .filter(masterDrive -> nonNull(masterDrive.getResourceContext()))
                .map(RequestedMasterDrive::getResourceContext)
                .collect(toList());
    }

    private void validateContexts(Set<Context> contexts) throws RequestValidationException {
        Violations violations = new Violations();

        for (Context context : contexts) {
            try {
                traverser.traverse(context);
            } catch (EntityNotFoundException e) {
                violations.addViolation("Specified resource (" + context + ") does not exist.");
            }
        }

        if (violations.hasViolations()) {
            throw new RequestValidationException(violations);
        }
    }
}
