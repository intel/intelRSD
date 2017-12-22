/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.intel.podm.business.Violations;
import com.intel.podm.business.entities.EntityNotFoundException;
import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.EthernetInterface;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.SimpleStorage;
import com.intel.podm.business.entities.redfish.Storage;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.redfish.services.allocation.AllocationRequestProcessingException;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.Id;
import org.apache.commons.collections.CollectionUtils;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.podm.business.redfish.services.allocation.strategy.ContextPossessorUtils.getAllChassisContexts;
import static com.intel.podm.business.redfish.services.allocation.strategy.ContextPossessorUtils.getAllResourceContexts;
import static com.intel.podm.business.redfish.services.allocation.strategy.ContextPossessorUtils.getChassisContexts;
import static com.intel.podm.business.redfish.services.allocation.strategy.ContextPossessorUtils.getLocalStorageUnderneathComputerSystemResourceContexts;
import static com.intel.podm.business.redfish.services.allocation.validation.Predicates.byExactInterface;
import static com.intel.podm.business.redfish.services.allocation.validation.Predicates.byPcieSwitch;
import static com.intel.podm.business.redfish.services.allocation.validation.Predicates.byPortPredicate;
import static com.intel.podm.common.types.Protocol.NVME;
import static java.util.Collections.disjoint;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

@Dependent
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemCollector {
    @Inject
    private GenericDao genericDao;

    @Inject
    private LocalStorageCollector localStorageCollector;

    @Inject
    private ComputerSystemDao computerSystemDao;

    public Set<ComputerSystem> collectDistinctComputerSystemsFromResourceContexts(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        Set<Context> contexts = new HashSet<>();
        contexts.addAll(getAllResourceContexts(requestedNode));
        contexts.addAll(getLocalStorageUnderneathComputerSystemResourceContexts(requestedNode.getLocalDrives()));
        return getComputerSystemsFromRequestedResourceContexts(contexts);
    }

    public Set<ComputerSystem> collectCommonComputerSystemsFromChassisContexts(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        List<Set<ComputerSystem>> listOfComputerSystemSets = new ArrayList<>();

        listOfComputerSystemSets.addAll(getComputerSystemsFromLocalDrives(getLocalDrivesFromChassisContext(requestedNode)));
        listOfComputerSystemSets.addAll(getCommonComputerSystemsFromRequestedChassisContexts(
            getAllChassisContexts(requestedNode)
        ));

        return getCommonComputerSystems(listOfComputerSystemSets);
    }

    private List<Set<ComputerSystem>> getComputerSystemsFromLocalDrives(Set<Drive> localDrives) throws AllocationRequestProcessingException {
        List<Set<ComputerSystem>> listOfComputerSystemSets = new ArrayList<>();

        listOfComputerSystemSets.add(getComputerSystemsFromNonNvmeDrives(localDrives));
        listOfComputerSystemSets.add(getComputerSystemsFromNvmeDrives(localDrives));
        listOfComputerSystemSets.removeIf(CollectionUtils::isEmpty);

        return listOfComputerSystemSets;
    }

    private Set<ComputerSystem> getComputerSystemsFromNonNvmeDrives(Set<Drive> localDrives) throws AllocationRequestProcessingException {
        Predicate<Drive> nonNvmeDrive = byExactInterface(NVME).negate();
        return localDrives.stream()
            .filter(nonNvmeDrive)
            .map(Drive::getStorage)
            .filter(Objects::nonNull)
            .map(Storage::getComputerSystem)
            .filter(Objects::nonNull)
            .collect(toSet());
    }

    private Set<ComputerSystem> getComputerSystemsFromNvmeDrives(Set<Drive> localDrives) throws AllocationRequestProcessingException {
        List<ComputerSystem> computerSystems = computerSystemDao.getComputerSystemsPossibleToAllocate();
        Predicate<Drive> nvmeDrive = byExactInterface(NVME);
        return localDrives.stream()
            .filter(nvmeDrive)
            .map(Drive::getChassis)
            .map(Chassis::getSwitch)
            .flatMap(Collection::stream)
            .filter(byPcieSwitch())
            .map(Switch::getPorts)
            .flatMap(Collection::stream)
            .filter(byPortPredicate())
            .map(port -> getComputerSystemsForPort(computerSystems, port))
            .flatMap(Collection::stream)
            .collect(toSet());
    }

    private Set<ComputerSystem> getComputerSystemsForPort(List<ComputerSystem> computerSystems, Port port) {
        List<String> connectionIds = port.getPcieConnectionIds();

        return computerSystems.stream()
            .filter(cs -> !disjoint(cs.getPcieConnectionIds(), connectionIds))
            .collect(toSet());
    }

    private Set<ComputerSystem> getComputerSystemsFromRequestedResourceContexts(Set<Context> contexts) throws AllocationRequestProcessingException {
        Set<ComputerSystem> computerSystems = new HashSet<>();
        Violations violations = new Violations();

        for (Context context : contexts) {
            try {
                Optional<ComputerSystem> computerSystem = getComputerSystemFromResource(context.getType(), context.getId());
                if (computerSystem.isPresent()) {
                    computerSystems.add(computerSystem.get());
                } else {
                    violations.addViolation("Specified resource (" + context + ") is not valid.");
                }
            } catch (EntityNotFoundException e) {
                violations.addViolation("Specified resource (" + context + ") does not exist.");
            }
        }

        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }

        return computerSystems;
    }


    private List<Set<ComputerSystem>> getCommonComputerSystemsFromRequestedChassisContexts(Set<Context> chassisContexts)
        throws AllocationRequestProcessingException {
        List<Set<ComputerSystem>> allComputerSystems = new ArrayList<>();

        allComputerSystems.addAll(
            getChassisFromContexts(chassisContexts).stream()
                .map(Chassis::getAllComputerSystemsUnderneath)
                .collect(toList()));

        return allComputerSystems;
    }

    private Set<ComputerSystem> getCommonComputerSystems(List<Set<ComputerSystem>> allComputerSystems) throws AllocationRequestProcessingException {
        Set<ComputerSystem> commonComputerSystems = new HashSet<>();
        Violations violations = new Violations();

        for (Set<ComputerSystem> computerSystems : allComputerSystems) {
            if (isEmpty(computerSystems)) {
                throw new AllocationRequestProcessingException(
                    violations.addViolation("Allocation of assets on chassis without computer system is not supported.")
                );
            }

            if (isEmpty(commonComputerSystems)) {
                commonComputerSystems.addAll(computerSystems);
            } else {
                commonComputerSystems.retainAll(computerSystems);
                if (isEmpty(commonComputerSystems)) {
                    throw new AllocationRequestProcessingException(
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

    private Set<Chassis> getChassisFromContexts(Set<Context> chassisContexts) throws AllocationRequestProcessingException {
        Set<Chassis> chassisSet = new HashSet<>();
        Violations violations = new Violations();

        for (Context chassisContext : chassisContexts) {
            chassisSet.add(genericDao.tryFind(Chassis.class, chassisContext.getId()).orElseThrow(() ->
                new AllocationRequestProcessingException(
                    violations.addViolation("Specified chassis (" + chassisContext + ") does not exist.")
                )
            ));
        }

        return chassisSet;
    }

    private Set<Drive> getLocalDrivesFromChassisContext(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        return localStorageCollector.getLocalDrivesFromChassis(
            getChassisFromContexts(getChassisContexts(requestedNode.getLocalDrives()))
        );
    }
}
