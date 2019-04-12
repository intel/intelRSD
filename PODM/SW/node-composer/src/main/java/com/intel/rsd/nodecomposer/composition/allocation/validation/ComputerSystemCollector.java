/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.allocation.validation;

import com.intel.rsd.nodecomposer.business.Violations;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.business.services.redfish.requests.RequestedNode;
import com.intel.rsd.nodecomposer.composition.allocation.AllocationRequestProcessingException;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.ChassisCollector;
import com.intel.rsd.nodecomposer.composition.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.rsd.nodecomposer.persistence.EntityNotFoundException;
import com.intel.rsd.nodecomposer.persistence.dao.ComputerSystemDao;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.dao.ProcessorDao;
import com.intel.rsd.nodecomposer.persistence.redfish.Chassis;
import com.intel.rsd.nodecomposer.persistence.redfish.ComputerSystem;
import com.intel.rsd.nodecomposer.persistence.redfish.Drive;
import com.intel.rsd.nodecomposer.persistence.redfish.EthernetInterface;
import com.intel.rsd.nodecomposer.persistence.redfish.Memory;
import com.intel.rsd.nodecomposer.persistence.redfish.Port;
import com.intel.rsd.nodecomposer.persistence.redfish.Processor;
import com.intel.rsd.nodecomposer.persistence.redfish.SimpleStorage;
import com.intel.rsd.nodecomposer.persistence.redfish.Storage;
import com.intel.rsd.nodecomposer.persistence.redfish.Switch;
import com.intel.rsd.nodecomposer.types.SystemType;
import org.apache.commons.collections.CollectionUtils;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.rsd.nodecomposer.composition.allocation.strategy.ODataIdPossessorUtils.getAllChassisODataIds;
import static com.intel.rsd.nodecomposer.composition.allocation.strategy.ODataIdPossessorUtils.getAllResourceODataIds;
import static com.intel.rsd.nodecomposer.composition.allocation.strategy.ODataIdPossessorUtils.getLocalStorageUnderneathComputerSystemResourceODataIds;
import static com.intel.rsd.nodecomposer.composition.allocation.validation.Predicates.byExactInterface;
import static com.intel.rsd.nodecomposer.composition.allocation.validation.Predicates.byPcieSwitch;
import static com.intel.rsd.nodecomposer.composition.allocation.validation.Predicates.byPortPredicate;
import static com.intel.rsd.nodecomposer.types.Protocol.NVME;
import static com.intel.rsd.nodecomposer.types.SystemType.PHYSICAL;
import static com.intel.rsd.nodecomposer.types.SystemType.VIRTUAL;
import static java.util.Collections.disjoint;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static org.apache.commons.collections.CollectionUtils.isEmpty;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class ComputerSystemCollector {
    private final GenericDao genericDao;
    private final LocalStorageCollector localStorageCollector;
    private final ComputerSystemDao computerSystemDao;
    private final ChassisCollector chassisCollector;
    private final ProcessorDao processorDao;

    @Autowired
    public ComputerSystemCollector(GenericDao genericDao, LocalStorageCollector localStorageCollector, ComputerSystemDao computerSystemDao,
                                   ChassisCollector chassisCollector, ProcessorDao processorDao) {
        this.genericDao = genericDao;
        this.localStorageCollector = localStorageCollector;
        this.computerSystemDao = computerSystemDao;
        this.chassisCollector = chassisCollector;
        this.processorDao = processorDao;
    }

    public Set<ComputerSystem> collectDistinctComputerSystemsFromResourceODataIds(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        Set<ODataIdToODataIdPossessorTypeMapping> oDataIds = new HashSet<>();
        oDataIds.addAll(getAllResourceODataIds(requestedNode));
        oDataIds.addAll(getLocalStorageUnderneathComputerSystemResourceODataIds(requestedNode.getLocalDrives()));
        return getComputerSystemsFromRequestedResourceODataIds(oDataIds);
    }

    public Set<ComputerSystem> collectCommonComputerSystemsFromChassisODataIds(RequestedNode requestedNode) throws AllocationRequestProcessingException {
        List<Set<ComputerSystem>> listOfComputerSystemSets = new ArrayList<>();

        listOfComputerSystemSets.addAll(getComputerSystemsFromLocalDrives(localStorageCollector.getLocalDrivesFromChassisODataId(requestedNode)));
        listOfComputerSystemSets.addAll(getCommonComputerSystemsFromRequestedChassisODataIds(getAllChassisODataIds(requestedNode)));

        return getCommonComputerSystems(listOfComputerSystemSets);
    }

    public ComputerSystem getComputerSystemByResourceUris(RequestedNode requestedNode) {
        try {
            Set<ComputerSystem> computerSystems = collectDistinctComputerSystemsFromResourceODataIds(requestedNode);
            ComputerSystem virtualSystem = retrieveSystemByType(computerSystems, VIRTUAL);
            ComputerSystem requestedPhysicalSystem = retrieveSystemByType(computerSystems, PHYSICAL);

            if (virtualSystem != null) {
                return getAchievableComputerSystem(virtualSystem, requestedPhysicalSystem, requestedNode.getProcessors());
            }

            return computerSystems.isEmpty() ? null : computerSystems.iterator().next();
        } catch (AllocationRequestProcessingException e) {
            throw new IllegalStateException("Some of provided resources are not valid", e);
        }
    }

    private List<Set<ComputerSystem>> getComputerSystemsFromLocalDrives(Set<Drive> localDrives) {
        List<Set<ComputerSystem>> listOfComputerSystemSets = new ArrayList<>();

        listOfComputerSystemSets.add(getComputerSystemsFromNonNvmeDrives(localDrives));
        listOfComputerSystemSets.add(getComputerSystemsFromNvmeDrives(localDrives));
        listOfComputerSystemSets.removeIf(CollectionUtils::isEmpty);

        return listOfComputerSystemSets;
    }

    private Set<ComputerSystem> getComputerSystemsFromNonNvmeDrives(Set<Drive> localDrives) {
        Predicate<Drive> nonNvmeDrive = byExactInterface(NVME).negate();
        return localDrives.stream()
            .filter(nonNvmeDrive)
            .map(Drive::getStorage)
            .filter(Objects::nonNull)
            .map(Storage::getComputerSystem)
            .filter(Objects::nonNull)
            .collect(toSet());
    }

    private Set<ComputerSystem> getComputerSystemsFromNvmeDrives(Set<Drive> localDrives) {
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

    private Set<ComputerSystem> getComputerSystemsFromRequestedResourceODataIds(
        Set<ODataIdToODataIdPossessorTypeMapping> oDataIdToODataIdPossessorTypeMappings) throws AllocationRequestProcessingException {

        Set<ComputerSystem> computerSystems = new HashSet<>();
        Violations violations = new Violations();

        for (ODataIdToODataIdPossessorTypeMapping oDataIdToODataIdPossessorTypeMapping : oDataIdToODataIdPossessorTypeMappings) {
            try {
                Optional<ComputerSystem> computerSystem = getComputerSystemFromResource(oDataIdToODataIdPossessorTypeMapping);
                if (computerSystem.isPresent()) {
                    computerSystems.add(computerSystem.get());
                } else {
                    violations.addViolation("Specified resource (" + oDataIdToODataIdPossessorTypeMapping + ") is not valid.");
                }
            } catch (EntityNotFoundException e) {
                violations.addViolation("Specified resource (" + oDataIdToODataIdPossessorTypeMapping + ") does not exist.");
            }
        }

        if (violations.hasViolations()) {
            throw new AllocationRequestProcessingException(violations);
        }

        return computerSystems;
    }

    private List<Set<ComputerSystem>> getCommonComputerSystemsFromRequestedChassisODataIds(Set<ODataId> chassisODataIds)
        throws AllocationRequestProcessingException {
        List<Set<ComputerSystem>> allComputerSystems = new ArrayList<>();

        allComputerSystems.addAll(
            chassisCollector.getChassisFromODataIds(chassisODataIds).stream()
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

    private Optional<ComputerSystem> getComputerSystemFromResource(ODataIdToODataIdPossessorTypeMapping oDataIdToODataIdPossessorTypeMapping) {
        Optional<ComputerSystem> computerSystem;

        switch (oDataIdToODataIdPossessorTypeMapping.getODataIdPossessorType()) {
            case PROCESSOR:
                computerSystem = of(genericDao.find(Processor.class, oDataIdToODataIdPossessorTypeMapping.getODataId()).getComputerSystem());
                break;
            case MEMORY:
                computerSystem = of(genericDao.find(Memory.class, oDataIdToODataIdPossessorTypeMapping.getODataId()).getComputerSystem());
                break;
            case SIMPLE_STORAGE:
                computerSystem = of(genericDao.find(SimpleStorage.class, oDataIdToODataIdPossessorTypeMapping.getODataId()).getComputerSystem());
                break;
            case ETHERNET_INTERFACE:
                computerSystem = of(genericDao.find(EthernetInterface.class, oDataIdToODataIdPossessorTypeMapping.getODataId()).getComputerSystem());
                break;
            default:
                computerSystem = empty();
        }

        return computerSystem;
    }

    private ComputerSystem getAchievableComputerSystem(ComputerSystem virtualSystem,
                                                       ComputerSystem requestedPhysicalSystem,
                                                       List<RequestedNode.Processor> processors) {
        Set<Processor> requestedProcessors = processorDao.retrieveRequestedFpgaProcessors(processors);

        if (requestedPhysicalSystem != null) {
            return processorDao.getAchievablePcieProcessors(requestedPhysicalSystem).containsAll(requestedProcessors) ? requestedPhysicalSystem : null;
        }

        Set<ComputerSystem> achievableComputerSystemsForFpgaSystem = computerSystemDao.getPhysicalComputerSystemsAchievableFromVirtualSystem(virtualSystem);

        Set<ComputerSystem> achievableComputerSystems = achievableComputerSystemsForFpgaSystem.stream()
            .filter(computerSystem -> processorDao.getAchievablePcieProcessors(computerSystem).containsAll(requestedProcessors))
            .collect(toSet());

        return achievableComputerSystems.isEmpty() ? null : achievableComputerSystems.iterator().next();
    }

    private ComputerSystem retrieveSystemByType(Collection<ComputerSystem> computerSystems, SystemType systemType) {
        return computerSystems.stream()
            .filter(computerSystem -> systemType.equals(computerSystem.getSystemType()))
            .findFirst()
            .orElse(null);
    }
}
