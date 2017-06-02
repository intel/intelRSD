/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.ContextResolvingException;
import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Id;
import org.testng.annotations.Test;

import java.util.Set;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class EntityTreeTraverserTest {
    @Test
    public void whenTraversingPodContext_shouldReturnEntityFromRepository() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();

        when(sut.validator.isValid(any())).thenReturn(true);

        when(sut.mapper.get(any())).thenReturn((Class) Processor.class);

        Processor expectedProcessor = mock(Processor.class);
        when(sut.genericDao.find(any(), any())).thenReturn(expectedProcessor);

        Context podContext = contextOf(id(1), PROCESSOR);
        Entity actualEntity = sut.traverse(podContext);

        assertEquals(actualEntity, expectedProcessor);
    }

    @Test(expectedExceptions = ContextResolvingException.class)
    public void whenTraversingInvalidProcessorContext_shouldThrow() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();

        Context processorContext = contextOf(id(1), PROCESSOR);
        when(sut.validator.isValid(processorContext)).thenReturn(false);

        sut.traverse(processorContext);
    }

    @Test
    public void whenTraversingBladeContext_shouldUseContextTypeToEntityClassMapperWithRepositoryTogether() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();
        when(sut.validator.isValid(any())).thenReturn(true);

        when(sut.mapper.get(PROCESSOR)).thenReturn((Class) MockedProcessor.class);

        Context processorContext = contextOf(id(2), COMPUTER_SYSTEM)
            .child(id(3), PROCESSOR);

        sut.traverse(processorContext);

        verify(sut.genericDao).find(eq(MockedProcessor.class), any());
    }

    @Test
    public void whenTraversingDifferentContexts_shouldUseContextTypeToEntityClassMapperWithCorrectParameters() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();
        when(sut.validator.isValid(any())).thenReturn(true);

        when(sut.mapper.get(ETHERNET_SWITCH_PORT)).thenReturn((Class) MockedSwitchPort.class);
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
    public void whenTraversingProcessorContext_ShouldUseRepositoryWithCorrectParameters() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();

        when(sut.validator.isValid(any())).thenReturn(true);
        when(sut.mapper.get(any())).thenReturn((Class) MockedProcessor.class);

        Context processorContext = contextOf(id(10), COMPUTER_SYSTEM).child(id(20), PROCESSOR);

        sut.traverse(processorContext);

        verify(sut.genericDao).find(MockedProcessor.class, id(20));
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenTraversingWithNullContext_shouldThrow() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();
        sut.traverse((Context) null);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenTraversingWithNullContextList_shouldThrow() throws ContextResolvingException {
        EntityTreeTraverser sut = createEntityTreeTraverser();
        sut.traverse((Set<Context>) null);
    }

    private EntityTreeTraverser createEntityTreeTraverser() {
        EntityTreeTraverser sut = new EntityTreeTraverser();
        sut.validator = mock(ContextValidator.class);
        sut.genericDao = mock(GenericDao.class);
        sut.mapper = mock(ContextTypeToEntityMapper.class);
        return sut;
    }

    public static class MockedProcessor extends DiscoverableEntity {
        @Override
        public void setId(Id id) {
        }

        @Override
        public Id getId() {
            return null;
        }

        @Override
        public void preRemove() {
        }

        @Override
        public boolean containedBy(Entity possibleParent) {
            return false;
        }
    }

    public static class MockedSwitchPort extends DiscoverableEntity {
        @Override
        public void setId(Id id) {
        }

        @Override
        public Id getId() {
            return null;
        }

        @Override
        public void preRemove() {
        }

        @Override
        public boolean containedBy(Entity possibleParent) {
            return false;
        }
    }
}
