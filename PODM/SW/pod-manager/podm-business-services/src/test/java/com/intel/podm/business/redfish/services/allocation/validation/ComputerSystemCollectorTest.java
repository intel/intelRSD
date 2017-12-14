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

import com.intel.podm.business.entities.dao.ComputerSystemDao;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Drive;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.Port;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.Switch;
import com.intel.podm.business.redfish.services.allocation.AllocationRequestProcessingException;
import com.intel.podm.business.redfish.services.allocation.strategy.matcher.LocalStorageCollector;
import com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeBuilder;
import com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeWithLocalDrives;
import com.intel.podm.business.redfish.services.allocation.templates.requestednode.RequestedNodeWithProcessorsAndMemoryModules;
import com.intel.podm.business.services.redfish.requests.RequestedNode;
import com.intel.podm.common.types.Id;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.util.List;
import java.util.Set;

import static com.intel.podm.business.redfish.services.allocation.templates.assets.LocalDrivesCreation.createRequestedDriveWithChassisContext;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.MemoryModulesCreation.createRequestedMemory;
import static com.intel.podm.business.redfish.services.allocation.templates.assets.ProcessorsCreation.createRequestedProcessor;
import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.types.MediaType.SSD;
import static com.intel.podm.common.types.PciePortType.UPSTREAM_PORT;
import static com.intel.podm.common.types.Protocol.NVME;
import static com.intel.podm.common.types.Protocol.PCIE;
import static java.util.Arrays.asList;
import static java.util.Collections.emptySet;
import static java.util.Collections.singleton;
import static java.util.Collections.singletonList;
import static java.util.Optional.of;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MagicNumber", "checkstyle:MethodName", "checkstyle:MethodLength",
    "checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class ComputerSystemCollectorTest {
    private static final String CONNECTION_ID = "XYZ1234567890";

    @InjectMocks
    private ComputerSystemCollector computerSystemCollector;
    @Mock
    private GenericDao genericDao;
    @Mock
    private LocalStorageCollector localStorageCollector;
    @Mock
    private ComputerSystemDao computerSystemDao;

    @BeforeMethod
    public void initializeMocks() {
        initMocks(this);
    }

    @Test
    public void whenRequestingNodeWithNoResourceContexts_shouldReturnNoComputerSystems() throws AllocationRequestProcessingException {
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(new RequestedNodeBuilder());
        assertTrue(computerSystems.isEmpty());
    }

    @Test
    public void whenRequestingNodeWithResourceContextsOnSingleComputerSystem_shouldReturnOneComputerSystem() throws AllocationRequestProcessingException {
        Id processor1Id = id(1);
        Id processor2Id = id(2);
        Id memory1Id = id(3);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, contextOf(processor1Id, PROCESSOR)),
            createRequestedProcessor(null, null, null, null, null, contextOf(processor2Id, PROCESSOR))
        ), singletonList(
            createRequestedMemory(null, null, null, null, null, contextOf(memory1Id, MEMORY))
        ));

        ComputerSystem computerSystem = mockComputerSystemWithTwoProcessorsAndOneMemory(processor1Id, processor2Id, memory1Id);
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(requestedNode);
        assertTrue(computerSystems.size() == 1);
        assertTrue(computerSystems.contains(computerSystem));
    }

    @Test
    public void whenRequestingNodeWithResourceContextsOnTwoComputerSystems_shouldReturnTwoComputerSystems() throws AllocationRequestProcessingException {
        Id processor1Id = id(1);
        Id processor2Id = id(2);
        Id memory1Id = id(3);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, contextOf(processor1Id, PROCESSOR)),
            createRequestedProcessor(null, null, null, null, null, contextOf(processor2Id, PROCESSOR))
        ), singletonList(
            createRequestedMemory(null, null, null, null, null, contextOf(memory1Id, MEMORY))
        ));

        ComputerSystem computerSystemWithProcessors = mockComputerSystemWithTwoProcessors(processor1Id, processor2Id);
        ComputerSystem computerSystemWithMemory = mockComputerSystemWithOneMemory(memory1Id);
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(requestedNode);
        assertTrue(computerSystems.size() == 2);
        assertTrue(computerSystems.contains(computerSystemWithProcessors));
        assertTrue(computerSystems.contains(computerSystemWithMemory));
    }

    @Test
    public void whenRequestingNodeWithNoChassisContexts_shouldReturnNoComputerSystems() throws AllocationRequestProcessingException {
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(new RequestedNodeBuilder());
        assertTrue(computerSystems.isEmpty());
    }

    @Test
    public void whenRequestingNodeWithChassisContextsContainingSingleComputerSystem_shouldReturnOneComputerSystem()
        throws AllocationRequestProcessingException {
        Id podChassisId = id(10);
        Id rackChassisId = id(11);
        Id drawerChassisId = id(12);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, null, contextOf(podChassisId, CHASSIS)),
            createRequestedProcessor(null, null, null, null, null, null, contextOf(rackChassisId, CHASSIS))
        ), singletonList(
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassisId, CHASSIS))
        ));

        ComputerSystem computerSystem =
            mockComputerSystemWithTwoProcessorsAndOneMemoryIndirectlyUnderPodChassis(podChassisId, rackChassisId, drawerChassisId);
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
        assertTrue(computerSystems.size() == 1);
        assertTrue(computerSystems.contains(computerSystem));
    }

    @Test(expectedExceptions = AllocationRequestProcessingException.class)
    public void whenRequestingNodeWithChassisContexts_OneOfThemDoesNotContainComputerSystem_shouldThrow() throws AllocationRequestProcessingException {
        Id podChassisId = id(10);
        Id rackChassisId = id(11);
        Id drawerChassis1Id = id(12);
        Id drawerChassis2Id = id(13);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, null, contextOf(podChassisId, CHASSIS)),
            createRequestedProcessor(null, null, null, null, null, null, contextOf(rackChassisId, CHASSIS))
        ), asList(
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassis1Id, CHASSIS)),
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassis2Id, CHASSIS))
        ));

        mockComputerSystemWithTwoProcessorsAndOneMemoryIndirectlyUnderPodChassis(podChassisId, rackChassisId, drawerChassis1Id);
        mockDrawerChassisWithoutComputerSystemsUnderneath(drawerChassis2Id);

        computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
    }

    @Test
    public void whenRequestingNodeWithChassisContexts_OneOfThemContainsNvmeDriveButDoesNotContainComputerSystem_shouldReturnOneComputerSystem()
        throws AllocationRequestProcessingException {
        Id drawerChassisId = id(12);

        ComputerSystem computerSystem = mockComputerSystemWithOneMemory(id(1));
        doReturn(singletonList(CONNECTION_ID)).when(computerSystem).getPcieConnectionIds();
        mockComputerSystems(singletonList(computerSystem));

        mockDrawerChassisWithNvmeDriveWithoutComputerSystem(drawerChassisId, computerSystem);

        RequestedNode requestedNode = new RequestedNodeWithLocalDrives(
            singletonList(createRequestedDriveWithChassisContext(SSD, 9999, NVME, null, null, null, contextOf(drawerChassisId, CHASSIS)))
        );
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
        assertTrue(computerSystems.size() == 1);
        assertTrue(computerSystems.contains(computerSystem));
    }

    @Test(expectedExceptions = AllocationRequestProcessingException.class)
    public void whenRequestingNodeWithChassisContexts_OneComputerSystemOnDifferentChassis_shouldThrow() throws AllocationRequestProcessingException {
        Id podChassisId = id(10);
        Id rackChassisId = id(11);
        Id drawerChassis1Id = id(12);
        Id drawerChassis2Id = id(13);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, null, contextOf(podChassisId, CHASSIS)),
            createRequestedProcessor(null, null, null, null, null, null, contextOf(rackChassisId, CHASSIS))
        ), asList(
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassis1Id, CHASSIS)),
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassis2Id, CHASSIS))
        ));

        mockComputerSystemWithTwoProcessorsAndOneMemoryIndirectlyUnderPodChassis(podChassisId, rackChassisId, drawerChassis1Id);
        mockComputerSystemWithOneMemoryIndirectlyUnderDrawerChassis(drawerChassis2Id);

        computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
    }

    @Test(expectedExceptions = AllocationRequestProcessingException.class)
    public void whenRequestingNodeWithChassisContexts_MemoryNProcessorOnDifferentComputerSystems_shouldThrow() throws AllocationRequestProcessingException {
        Id drawerChassis1Id = id(12);
        Id drawerChassis2Id = id(13);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(singletonList(
            createRequestedProcessor(null, null, null, null, null, null, contextOf(drawerChassis1Id, CHASSIS))
        ), singletonList(
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassis2Id, CHASSIS))
        ));

        mockComputerSystemWithOneProcessorIndirectlyUnderDrawerChassis(drawerChassis1Id);
        mockComputerSystemWithOneMemoryIndirectlyUnderDrawerChassis(drawerChassis2Id);

        computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
    }

    private ComputerSystem mockComputerSystemWithTwoProcessorsAndOneMemory(Id processor1Id, Id processor2Id, Id memoryId) {
        ComputerSystem computerSystem = mock(ComputerSystem.class);

        Processor mockedProcessor1 = mock(Processor.class);
        when(mockedProcessor1.getComputerSystem()).thenReturn(computerSystem);

        Processor mockedProcessor2 = mock(Processor.class);
        when(mockedProcessor2.getComputerSystem()).thenReturn(computerSystem);

        Memory mockedMemory = mock(Memory.class);
        when(mockedMemory.getComputerSystem()).thenReturn(computerSystem);

        when(genericDao.find(Processor.class, processor1Id)).thenReturn(mockedProcessor1);
        when(genericDao.find(Processor.class, processor2Id)).thenReturn(mockedProcessor2);
        when(genericDao.find(Memory.class, memoryId)).thenReturn(mockedMemory);

        return computerSystem;
    }

    private ComputerSystem mockComputerSystemWithTwoProcessors(Id processor1Id, Id processor2Id) {
        ComputerSystem computerSystem = mock(ComputerSystem.class);

        Processor mockedProcessor1 = mock(Processor.class);
        when(mockedProcessor1.getComputerSystem()).thenReturn(computerSystem);

        Processor mockedProcessor2 = mock(Processor.class);
        when(mockedProcessor2.getComputerSystem()).thenReturn(computerSystem);

        when(genericDao.find(Processor.class, processor1Id)).thenReturn(mockedProcessor1);
        when(genericDao.find(Processor.class, processor2Id)).thenReturn(mockedProcessor2);

        return computerSystem;
    }

    private ComputerSystem mockComputerSystemWithOneMemory(Id memoryId) {
        ComputerSystem computerSystem = mock(ComputerSystem.class);

        Memory mockedMemory = mock(Memory.class);
        when(mockedMemory.getComputerSystem()).thenReturn(computerSystem);

        when(genericDao.find(Memory.class, memoryId)).thenReturn(mockedMemory);

        return computerSystem;
    }

    private ComputerSystem mockComputerSystemWithOneProcessor(Id processorId) {
        ComputerSystem computerSystem = mock(ComputerSystem.class);

        Processor mockedProcessor = mock(Processor.class);
        when(mockedProcessor.getComputerSystem()).thenReturn(computerSystem);

        when(genericDao.find(Processor.class, processorId)).thenReturn(mockedProcessor);

        return computerSystem;
    }

    private ComputerSystem mockComputerSystemWithTwoProcessorsAndOneMemoryIndirectlyUnderPodChassis(
        Id podChassisId, Id rackChassisId, Id drawerChassisId) {

        Chassis podChassis = spy(new Chassis());
        Chassis rackChassis = spy(new Chassis());
        Chassis drawerChassis = spy(new Chassis());
        Chassis sledChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());
        ComputerSystem computerSystem = mockComputerSystemWithTwoProcessorsAndOneMemory(id(1), id(2), id(3));

        doReturn(emptySet()).when(podChassis).getComputerSystems();
        doReturn(singleton(rackChassis)).when(podChassis).getContainedChassis();

        doReturn(emptySet()).when(rackChassis).getComputerSystems();
        doReturn(singleton(drawerChassis)).when(rackChassis).getContainedChassis();

        doReturn(emptySet()).when(drawerChassis).getComputerSystems();
        doReturn(singleton(sledChassis)).when(drawerChassis).getContainedChassis();

        doReturn(emptySet()).when(sledChassis).getComputerSystems();
        doReturn(singleton(bladeChassis)).when(sledChassis).getContainedChassis();

        doReturn(singleton(computerSystem)).when(bladeChassis).getComputerSystems();
        doReturn(emptySet()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, podChassisId)).thenReturn(of(podChassis));
        when(genericDao.tryFind(Chassis.class, rackChassisId)).thenReturn(of(rackChassis));
        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));

        return computerSystem;
    }

    private void mockDrawerChassisWithNvmeDriveWithoutComputerSystem(Id drawerChassisId, ComputerSystem computerSystem) {
        Chassis drawerChassis = spy(new Chassis());
        Drive nvmeDrive = spy(new Drive());
        Switch pcieSwitch = spy(new Switch());
        Port pciePort = spy(new Port());

        doReturn(emptySet()).when(drawerChassis).getComputerSystems();
        doReturn(emptySet()).when(drawerChassis).getContainedChassis();
        doReturn(singleton(nvmeDrive)).when(drawerChassis).getDrives();
        doReturn(singleton(pcieSwitch)).when(drawerChassis).getSwitch();

        doReturn(NVME).when(nvmeDrive).getProtocol();
        doReturn(drawerChassis).when(nvmeDrive).getChassis();

        doReturn(PCIE).when(pcieSwitch).getSwitchType();
        doReturn(singleton(pciePort)).when(pcieSwitch).getPorts();
        doReturn(UPSTREAM_PORT).when(pciePort).getPortType();
        doReturn(singletonList(CONNECTION_ID)).when(pciePort).getPcieConnectionIds();

        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));
        when(localStorageCollector.getLocalDrivesFromChassis(singleton(drawerChassis)))
            .thenReturn(singleton(nvmeDrive));
    }

    private void mockDrawerChassisWithoutComputerSystemsUnderneath(Id drawerChassisId) {
        Chassis drawerChassis = spy(new Chassis());
        Chassis sledChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());

        doReturn(emptySet()).when(drawerChassis).getComputerSystems();
        doReturn(singleton(sledChassis)).when(drawerChassis).getContainedChassis();

        doReturn(emptySet()).when(sledChassis).getComputerSystems();
        doReturn(singleton(bladeChassis)).when(sledChassis).getContainedChassis();

        doReturn(emptySet()).when(bladeChassis).getComputerSystems();
        doReturn(emptySet()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));
    }

    private ComputerSystem mockComputerSystemWithOneMemoryIndirectlyUnderDrawerChassis(Id drawerChassisId) {
        Chassis drawerChassis = spy(new Chassis());
        Chassis sledChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());
        ComputerSystem computerSystem = mockComputerSystemWithOneMemory(id(4));

        doReturn(emptySet()).when(drawerChassis).getComputerSystems();
        doReturn(singleton(sledChassis)).when(drawerChassis).getContainedChassis();

        doReturn(emptySet()).when(sledChassis).getComputerSystems();
        doReturn(singleton(bladeChassis)).when(sledChassis).getContainedChassis();

        doReturn(singleton(computerSystem)).when(bladeChassis).getComputerSystems();
        doReturn(emptySet()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));

        return computerSystem;
    }

    private ComputerSystem mockComputerSystemWithOneProcessorIndirectlyUnderDrawerChassis(Id drawerChassisId) {
        Chassis drawerChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());
        ComputerSystem computerSystem = mockComputerSystemWithOneProcessor(id(4));

        doReturn(emptySet()).when(drawerChassis).getComputerSystems();
        doReturn(singleton(bladeChassis)).when(drawerChassis).getContainedChassis();

        doReturn(singleton(computerSystem)).when(bladeChassis).getComputerSystems();
        doReturn(emptySet()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));

        return computerSystem;
    }

    private void mockComputerSystems(List<ComputerSystem> computerSystemList) {
        when(computerSystemDao.getComputerSystemsPossibleToAllocate()).thenReturn(computerSystemList);
    }
}
