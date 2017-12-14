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

package com.intel.podm.business.services.context;

import org.testng.annotations.Test;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.PathParamConstants.getPathParameterNames;
import static java.util.Arrays.asList;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ParamNameContextTypeMapperTest {
    @Test
    public void whenGettingMappedContextTypeForComputerSystemId_shouldReturnCorrectContextType() {
        ParamNameContextTypeMapper sut = new ParamNameContextTypeMapper();

        ContextType actualContextType = sut.get("{computerSystemId}");
        assertEquals(actualContextType, COMPUTER_SYSTEM);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*sdahjdkshadj.*")
    public void whenGettingMappedContextTypeForUnknownParamName_shouldThrow() {
        ParamNameContextTypeMapper sut = new ParamNameContextTypeMapper();
        sut.get("sdahjdkshadj");
    }

    @Test
    public void whenCheckingWhetherComputerSystemIdParamIsKnown_shouldReturnTrue() {
        ParamNameContextTypeMapper sut = new ParamNameContextTypeMapper();
        assertTrue(sut.contains("{computerSystemId}"));
    }

    @Test
    public void whenCheckingWhetherUnknownParamIsKnown_shouldReturnFalse() {
        ParamNameContextTypeMapper sut = new ParamNameContextTypeMapper();
        assertFalse(sut.contains("sdasdsad"));
    }

    @Test
    public void checkWhetherAllContextTypesAreMapped() {
        Set<ContextType> mappedContextTypes = new HashSet<>(ParamNameContextTypeMapper.MAPPING.values());

        Set<ContextType> allContextTypes = new HashSet<>(asList(ContextType.values()));
        assertEquals(mappedContextTypes, allContextTypes);
    }

    @Test
    public void checkWhetherMappingIsInjective() {
        Collection<ContextType> values = ParamNameContextTypeMapper.MAPPING.values();

        int numberOfMappedValues = values.size();
        int numberOfDistinctValues = new HashSet<>(values).size();

        assertEquals(numberOfMappedValues, numberOfDistinctValues);
    }

    @Test
    public void checkWhetherAllParametersAreMapped() {
        Set<String> mappedPathParams = ParamNameContextTypeMapper.MAPPING.keySet();
        Set<String> allPathParams = new HashSet<>(getPathParameterNames());
        assertEquals(mappedPathParams, allPathParams);
    }
}
