/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.business.redfish;

import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.context.Context;
import org.testng.annotations.Test;

import java.util.List;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.MEMORY_CHUNK;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

public class DomainObjectTreeTraverserTest {
    @Test
    public void whenTraversingPodContext_shouldReturnDomainObjectFromRepository() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();

        when(sut.validator.isValid(any())).thenReturn(true);

        when(sut.mapper.get(any())).thenReturn((Class) Processor.class);

        Processor expectedProcessor = mock(Processor.class);
        when(sut.genericDao.find(any(), any())).thenReturn(expectedProcessor);

        Context podContext = contextOf(id(1), PROCESSOR);
        DomainObject actualDomainObject = sut.traverse(podContext);

        assertEquals(actualDomainObject, expectedProcessor);
    }

    @Test(expectedExceptions = EntityNotFoundException.class)
    public void whenTraversingInvalidProcessorContext_shouldThrow() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();

        Context processorContext = contextOf(id(1), PROCESSOR);
        when(sut.validator.isValid(processorContext)).thenReturn(false);

        sut.traverse(processorContext);
    }

    @Test
    public void whenTraversingBladeContext_shouldUseContextTypeToDomainObjectClassMapperWithRepositoryTogether() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();
        when(sut.validator.isValid(any())).thenReturn(true);

        when(sut.mapper.get(PROCESSOR)).thenReturn((Class) MockedProcessor.class);

        Context processorContext = contextOf(id(2), COMPUTER_SYSTEM)
                .child(id(3), PROCESSOR);

        sut.traverse(processorContext);

        verify(sut.genericDao).find(eq(MockedProcessor.class), any());
    }

    @Test
    public void whenTraversingDifferentContexts_shouldUseContextTypeToDomainObjectClassMapperWithCorrectParameters() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();
        when(sut.validator.isValid(any())).thenReturn(true);

        when(sut.mapper.get(MEMORY_CHUNK)).thenReturn((Class) MockedMemoryChunk.class);
        when(sut.mapper.get(PROCESSOR)).thenReturn((Class) MockedProcessor.class);

        Context processorContext = contextOf(id(20), COMPUTER_SYSTEM)
                .child(id(30), PROCESSOR);

        Context ethernetSwitchPortContext = contextOf(id(22), ETHERNET_SWITCH)
                .child(id(33), ETHERNET_SWITCH_PORT);

        sut.traverse(processorContext);
        sut.traverse(ethernetSwitchPortContext);

        verify(sut.mapper).get(PROCESSOR);
        verify(sut.mapper).get(ETHERNET_SWITCH_PORT);
    }

    @Test
    public void whenTraversingRackContext_ShouldUseRepositoryWithCorrectParameters() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();

        when(sut.validator.isValid(any())).thenReturn(true);
        when(sut.mapper.get(any())).thenReturn((Class) MockedMemoryChunk.class);

        Context memoryChunkContext = contextOf(id(10), COMPUTER_SYSTEM).child(id(20), MEMORY_CHUNK);

        sut.traverse(memoryChunkContext);

        verify(sut.genericDao).find(MockedMemoryChunk.class, id(20));
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenTraversingWithNullContext_shouldThrow() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();
        sut.traverse((Context) null);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenTraversingWithNullContextList_shouldThrow() throws EntityNotFoundException {
        DomainObjectTreeTraverser sut = createDomainObjectTreeTraverser();
        sut.traverse((List<Context>) null);
    }

    private DomainObjectTreeTraverser createDomainObjectTreeTraverser() {
        DomainObjectTreeTraverser sut = new DomainObjectTreeTraverser();
        sut.validator = mock(ContextValidator.class);
        sut.genericDao = mock(GenericDao.class);
        sut.mapper = mock(ContextTypeToDomainObjectMapper.class);
        return sut;
    }

    public static class MockedProcessor extends DomainObject {
    }

    public static class MockedMemoryChunk extends DomainObject {
    }
}
