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
import com.intel.podm.business.dto.redfish.RequestedNode;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Chassis;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.entities.redfish.Memory;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.common.types.Id;
import com.intel.podm.templates.requestednode.RequestedNodeWithProcessorsAndMemoryModules;
import com.intel.podm.templates.requestednode.TestRequestedNode;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.util.Set;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.MEMORY;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.templates.assets.MemoryModulesCreation.createRequestedMemory;
import static com.intel.podm.templates.assets.ProcessorsCreation.createRequestedProcessor;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static java.util.Collections.singletonList;
import static java.util.Optional.of;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.testng.Assert.assertTrue;

public class ComputerSystemCollectorTest {
    @InjectMocks
    private ComputerSystemCollector computerSystemCollector;

    @Mock
    private GenericDao genericDao;

    @BeforeMethod
    public void initializeMocks() {
        initMocks(this);
    }

    @Test
    public void whenRequestingNodeWithNoResourceContexts_shouldReturnNoComputerSystems() throws RequestValidationException {
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(new TestRequestedNode());
        assertTrue(computerSystems.isEmpty());
    }

    @Test
    public void whenRequestingNodeWithResourceContextsOnSingleComputerSystem_shouldReturnOneComputerSystem() throws RequestValidationException {
        Id processor1Id = id(1);
        Id processor2Id = id(2);
        Id memory1Id = id(3);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, contextOf(processor1Id, PROCESSOR)),
            createRequestedProcessor(null, null, null, null, null, contextOf(processor2Id, PROCESSOR))
        ), asList(
            createRequestedMemory(null, null, null, null, null, contextOf(memory1Id, MEMORY))
        ));

        ComputerSystem computerSystem = mockComputerSystemWithTwoProcessorsAndOneMemory(processor1Id, processor2Id, memory1Id);
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(requestedNode);
        assertTrue(computerSystems.size() == 1);
        assertTrue(computerSystems.contains(computerSystem));
    }

    @Test
    public void whenRequestingNodeWithResourceContextsOnTwoComputerSystems_shouldReturnTwoComputerSystems() throws RequestValidationException {
        Id processor1Id = id(1);
        Id processor2Id = id(2);
        Id memory1Id = id(3);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, contextOf(processor1Id, PROCESSOR)),
            createRequestedProcessor(null, null, null, null, null, contextOf(processor2Id, PROCESSOR))
        ), asList(
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
    public void whenRequestingNodeWithNoChassisContexts_shouldReturnNoComputerSystems() throws RequestValidationException {
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectDistinctComputerSystemsFromResourceContexts(new TestRequestedNode());
        assertTrue(computerSystems.isEmpty());
    }

    @Test
    public void whenRequestingNodeWithChassisContextsContainingSingleComputerSystem_shouldReturnOneComputerSystem() throws RequestValidationException {
        Id podChassisId = id(10);
        Id rackChassisId = id(11);
        Id drawerChassisId = id(12);

        RequestedNode requestedNode = new RequestedNodeWithProcessorsAndMemoryModules(asList(
            createRequestedProcessor(null, null, null, null, null, null, contextOf(podChassisId, CHASSIS)),
            createRequestedProcessor(null, null, null, null, null, null, contextOf(rackChassisId, CHASSIS))
        ), asList(
            createRequestedMemory(null, null, null, null, null, null, contextOf(drawerChassisId, CHASSIS))
        ));

        ComputerSystem computerSystem =
                mockComputerSystemWithTwoProcessorsAndOneMemoryIndirectlyUnderPodChassis(podChassisId, rackChassisId, drawerChassisId);
        Set<ComputerSystem> computerSystems = computerSystemCollector.collectCommonComputerSystemsFromChassisContexts(requestedNode);
        assertTrue(computerSystems.size() == 1);
        assertTrue(computerSystems.contains(computerSystem));
    }

    @Test(expectedExceptions = RequestValidationException.class)
    public void whenRequestingNodeWithChassisContexts_OneOfThemDoesNotContainComputerSystem_shouldThrow() throws RequestValidationException {
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

    @Test(expectedExceptions = RequestValidationException.class)
    public void whenRequestingNodeWithChassisContexts_OneComputerSystemOnDifferentChassis_shouldThrow() throws RequestValidationException {
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

    private ComputerSystem mockComputerSystemWithTwoProcessorsAndOneMemoryIndirectlyUnderPodChassis(
            Id podChassisId, Id rackChassisId, Id drawerChassisId) {

        Chassis podChassis = spy(new Chassis());
        Chassis rackChassis = spy(new Chassis());
        Chassis drawerChassis = spy(new Chassis());
        Chassis sledChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());
        ComputerSystem computerSystem = mockComputerSystemWithTwoProcessorsAndOneMemory(id(1), id(2), id(3));

        doReturn(emptyList()).when(podChassis).getComputerSystems();
        doReturn(singletonList(rackChassis)).when(podChassis).getContainedChassis();

        doReturn(emptyList()).when(rackChassis).getComputerSystems();
        doReturn(singletonList(drawerChassis)).when(rackChassis).getContainedChassis();

        doReturn(emptyList()).when(drawerChassis).getComputerSystems();
        doReturn(singletonList(sledChassis)).when(drawerChassis).getContainedChassis();

        doReturn(emptyList()).when(sledChassis).getComputerSystems();
        doReturn(singletonList(bladeChassis)).when(sledChassis).getContainedChassis();

        doReturn(singletonList(computerSystem)).when(bladeChassis).getComputerSystems();
        doReturn(emptyList()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, podChassisId)).thenReturn(of(podChassis));
        when(genericDao.tryFind(Chassis.class, rackChassisId)).thenReturn(of(rackChassis));
        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));

        return computerSystem;
    }

    private void mockDrawerChassisWithoutComputerSystemsUnderneath(Id drawerChassisId) {
        Chassis drawerChassis = spy(new Chassis());
        Chassis sledChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());

        doReturn(emptyList()).when(drawerChassis).getComputerSystems();
        doReturn(singletonList(sledChassis)).when(drawerChassis).getContainedChassis();

        doReturn(emptyList()).when(sledChassis).getComputerSystems();
        doReturn(singletonList(bladeChassis)).when(sledChassis).getContainedChassis();

        doReturn(emptyList()).when(bladeChassis).getComputerSystems();
        doReturn(emptyList()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));
    }

    private ComputerSystem mockComputerSystemWithOneMemoryIndirectlyUnderDrawerChassis(Id drawerChassisId) {
        Chassis drawerChassis = spy(new Chassis());
        Chassis sledChassis = spy(new Chassis());
        Chassis bladeChassis = spy(new Chassis());
        ComputerSystem computerSystem = mockComputerSystemWithOneMemory(id(4));

        doReturn(emptyList()).when(drawerChassis).getComputerSystems();
        doReturn(singletonList(sledChassis)).when(drawerChassis).getContainedChassis();

        doReturn(emptyList()).when(sledChassis).getComputerSystems();
        doReturn(singletonList(bladeChassis)).when(sledChassis).getContainedChassis();

        doReturn(singletonList(computerSystem)).when(bladeChassis).getComputerSystems();
        doReturn(emptyList()).when(bladeChassis).getContainedChassis();

        when(genericDao.tryFind(Chassis.class, drawerChassisId)).thenReturn(of(drawerChassis));

        return computerSystem;
    }
}
