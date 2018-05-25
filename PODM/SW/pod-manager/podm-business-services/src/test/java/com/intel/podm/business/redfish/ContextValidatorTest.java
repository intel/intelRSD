/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.Manager;
import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.services.context.Context;
import org.testng.annotations.Test;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.MANAGER;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.common.types.Id.id;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:MethodName"})
public class ContextValidatorTest {
    /**
     * NUll parent context may be acceptable in some cases by design but it should not be considered as valid context in general.
     * If you need such functionality like checking whether context is valid or null, please provide new method "isValidOrNull".
     */
    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenValidatingNullContext_shouldThrow() {
        ContextValidator sut = createContextValidator();

        sut.isValid(null);
    }

    @Test
    public void whenValidatingContextOfExistingProcessor_shouldReturnTrue() {
        ContextValidator sut = createContextValidator();

        when(sut.genericDao.tryFind(Processor.class, id(2))).thenReturn(of(mock(Processor.class)));

        Context processorContext = contextOf(id(2), PROCESSOR);

        assertTrue(sut.isValid(processorContext));
    }

    @Test
    public void whenValidatingContextOfNonexistentProcessor_shouldReturnFalse() {
        ContextValidator sut = createContextValidator();

        when(sut.genericDao.tryFind(Processor.class, id(2))).thenReturn(empty());

        Context processorContext = contextOf(id(2), PROCESSOR);

        assertFalse(sut.isValid(processorContext));
    }

    @Test
    public void whenValidatingContextOfExistingManager_shouldReturnTrue() {
        ContextValidator sut = createContextValidator();

        when(sut.genericDao.tryFind(Manager.class, id(123))).thenReturn(of(mock(Manager.class)));

        Context managerContext = contextOf(id(123), MANAGER);

        assertTrue(sut.isValid(managerContext));
    }

    private ContextValidator createContextValidator() {
        ContextValidator sut = new ContextValidator();
        sut.genericDao = mock(GenericDao.class);

        sut.contextTypeToEntityMapper = mock(ContextTypeToEntityMapper.class);
        when(sut.contextTypeToEntityMapper.getEntityClass(PROCESSOR)).thenReturn((Class) Processor.class);
        when(sut.contextTypeToEntityMapper.getEntityClass(MANAGER)).thenReturn((Class) Manager.class);

        return sut;
    }
}
