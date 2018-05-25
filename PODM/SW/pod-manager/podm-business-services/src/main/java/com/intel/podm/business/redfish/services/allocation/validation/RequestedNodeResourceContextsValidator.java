/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.allocation.validation;

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.Violations;
import com.intel.podm.business.dto.redfish.ContextPossessor;
import com.intel.podm.business.redfish.EntityTreeTraverser;
import com.intel.podm.business.redfish.services.allocation.AllocationRequestProcessingException;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive;
import com.intel.podm.business.services.redfish.requests.RequestedNode.RemoteDrive.MasterDrive;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.util.Collections.emptyList;
import static java.util.Optional.ofNullable;
import static org.apache.commons.collections4.CollectionUtils.isEmpty;

@Dependent
public class RequestedNodeResourceContextsValidator {
    @Inject
    private EntityTreeTraverser traverser;

    public void validateExistenceOfIncludedResources(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        Set<Context> contexts = new HashSet<>();
        contexts.addAll(getContexts(requestedNode.getProcessors()));
        contexts.addAll(getContexts(requestedNode.getMemoryModules()));
        contexts.addAll(collectRemoteDriveContexts(requestedNode.getRemoteDrives()));
        contexts.addAll(getContexts(requestedNode.getLocalDrives()));
        contexts.addAll(getContexts(requestedNode.getEthernetInterfaces()));

        validateContexts(contexts);
    }


    private List<Context> getContexts(List<? extends ContextPossessor> contextPossessors) {
        if (isEmpty(contextPossessors)) {
            return emptyList();
        }

        List<Context> contexts = new ArrayList<>();
        contextPossessors.forEach(contextPossessor -> {
            if (contextPossessor.getResourceContext() != null) {
                contexts.add(contextPossessor.getResourceContext());
            }
            if (contextPossessor.getChassisContext() != null) {
                contexts.add(contextPossessor.getChassisContext());
            }
        });
        return contexts;
    }

    private List<Context> collectRemoteDriveContexts(List<RemoteDrive> remoteDrives) {
        if (isEmpty(remoteDrives)) {
            return emptyList();
        }

        List<Context> remoteDriveContexts = new ArrayList<>();
        remoteDrives.forEach(remoteDrive -> {
            ofNullable(remoteDrive.getResourceContext()).ifPresent(remoteDriveContexts::add);

            MasterDrive masterDrive = remoteDrive.getMaster();
            if (masterDrive != null) {
                ofNullable(masterDrive.getResourceContext()).ifPresent(remoteDriveContexts::add);
            }
        });

        return remoteDriveContexts;
    }

    private void validateContexts(Set<Context> contexts) throws AllocationRequestProcessingException {
        Violations violations = new Violations();

        for (Context context : contexts) {
            try {
                traverser.traverse(context);
            } catch (ContextResolvingException e) {
                violations.addViolation("Specified resource (" + context + ") does not exist.");
            }
        }

        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }
    }
}
