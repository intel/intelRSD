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

package com.intel.podm.allocation.validation;

import com.intel.podm.allocation.RequestValidationException;
import com.intel.podm.business.dto.redfish.ContextPossessor;
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.DomainObjectNotFoundException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Device;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.common.types.Id;

import javax.inject.Inject;
import java.util.List;
import java.util.Optional;
import java.util.Set;

import static com.google.common.collect.Lists.newArrayList;
import static com.google.common.collect.Sets.newHashSet;
import static java.util.Collections.emptySet;
import static java.util.Objects.nonNull;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

public class ComputerSystemCollector {
    @Inject
    private GenericDao genericDao;

    public Set<ComputerSystem> collectDistinctComputerSystemsFromResourceContexts(RequestedNode requestedNode) throws RequestValidationException {
        Set<Context> contexts = newHashSet();
        contexts.addAll(getResourceContexts(requestedNode.getProcessors()));
        contexts.addAll(getResourceContexts(requestedNode.getMemoryModules()));
        contexts.addAll(getResourceContexts(requestedNode.getLocalDrives()));
        contexts.addAll(getResourceContexts(requestedNode.getEthernetInterfaces()));
        return getComputerSystemsFromRequestedResourceContexts(contexts);
    }

    public Set<ComputerSystem> collectCommonComputerSystemsFromChassisContexts(RequestedNode requestedNode) throws RequestValidationException {
        return getCommonComputerSystemsFromRequestedChassisContexts(
                getChassisContexts(requestedNode.getProcessors()),
                getChassisContexts(requestedNode.getMemoryModules()),
                getChassisContexts(requestedNode.getLocalDrives()),
                getChassisContexts(requestedNode.getEthernetInterfaces())
        );
    }

    private Set<Context> getResourceContexts(List<? extends ContextPossessor> contextPossessors) {
        if (isEmpty(contextPossessors)) {
            return emptySet();
        }

        return contextPossessors.stream()
                .filter(contextPossessor -> nonNull(contextPossessor.getResourceContext()))
                .map(ContextPossessor::getResourceContext)
                .collect(toSet());
    }

    private Set<Context> getChassisContexts(List<? extends ContextPossessor> contextPossessors) {
        if (isEmpty(contextPossessors)) {
            return emptySet();
        }

        return contextPossessors.stream()
                .filter(contextPossessor -> nonNull(contextPossessor.getChassisContext()))
                .map(ContextPossessor::getChassisContext)
                .collect(toSet());
    }

    private Set<ComputerSystem> getComputerSystemsFromRequestedResourceContexts(Set<Context> contexts) throws RequestValidationException {
        Set<ComputerSystem> computerSystems = newHashSet();
        Violations violations = new Violations();

        for (Context context : contexts) {
            try {
                Optional<ComputerSystem> computerSystem = getComputerSystemFromResource(context.getType(), context.getId());
                if (computerSystem.isPresent()) {
                    computerSystems.add(computerSystem.get());
                } else {
                    violations.addViolation("Specified resource (" + context + ") is not valid.");
                }
            } catch (DomainObjectNotFoundException e) {
                violations.addViolation("Specified resource (" + context + ") does not exist.");
            }
        }

        if (violations.hasViolations()) {
            throw new RequestValidationException(violations);
        }

        return computerSystems;
    }

    @SafeVarargs
    private final Set<ComputerSystem> getCommonComputerSystemsFromRequestedChassisContexts(Set<Context>... chassisContextsArray)
            throws RequestValidationException {
        List<Set<ComputerSystem>> allComputerSystems = newArrayList();

        for (Set<Context> chassisContexts : chassisContextsArray) {
            allComputerSystems.addAll(
                    getChassisFromContexts(chassisContexts).stream()
                            .map(Chassis::getAllComputerSystemsUnderneath)
                            .collect(toList()));
        }

        return getCommonComputerSystems(allComputerSystems);
    }

    private Set<ComputerSystem> getCommonComputerSystems(List<Set<ComputerSystem>> allComputerSystems) throws RequestValidationException {
        Set<ComputerSystem> commonComputerSystems = newHashSet();
        Violations violations = new Violations();

        for (Set<ComputerSystem> computerSystems : allComputerSystems) {
            if (isEmpty(computerSystems)) {
                throw new RequestValidationException(
                        violations.addViolation("Allocation of assets on chassis without computer system is not supported.")
                );
            }

            if (isEmpty(commonComputerSystems)) {
                commonComputerSystems.addAll(computerSystems);
            } else {
                commonComputerSystems.retainAll(computerSystems);
                if (isEmpty(commonComputerSystems)) {
                    throw new RequestValidationException(
                            violations.addViolation("Allocation of assets on multiple chassis is not supported.")
                    );
                }
            }
        }

        return commonComputerSystems;
    }

    private Optional<ComputerSystem> getComputerSystemFromResource(ContextType contextType, Id resourceId) {
        Optional<ComputerSystem> computerSystem;

        switch (contextType) {
            case PROCESSOR:
                computerSystem = of(genericDao.find(Processor.class, resourceId).getComputerSystem());
                break;
            case MEMORY:
                computerSystem = of(genericDao.find(Memory.class, resourceId).getComputerSystem());
                break;
            case DEVICE:
                computerSystem = of(genericDao.find(Device.class, resourceId).getAdapter().getComputerSystem());
                break;
            case SIMPLE_STORAGE:
                computerSystem = of(genericDao.find(SimpleStorage.class, resourceId).getComputerSystem());
                break;
            case ETHERNET_INTERFACE:
                computerSystem = of(genericDao.find(EthernetInterface.class, resourceId).getComputerSystem());
                break;
            default:
                computerSystem = empty();
        }

        return computerSystem;
    }

    private Set<Chassis> getChassisFromContexts(Set<Context> chassisContexts) throws RequestValidationException {
        Set<Chassis> chassisSet = newHashSet();
        Violations violations = new Violations();

        for (Context chassisContext : chassisContexts) {
            chassisSet.add(genericDao.tryFind(Chassis.class, chassisContext.getId()).orElseThrow(() ->
                            new RequestValidationException(
                                    violations.addViolation("Specified chassis (" + chassisContext + ") does not exist.")
                            )
            ));
        }

        return chassisSet;
    }
}
