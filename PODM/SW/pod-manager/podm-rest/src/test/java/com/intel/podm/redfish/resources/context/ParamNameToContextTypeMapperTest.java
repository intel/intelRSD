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

package com.intel.podm.redfish.resources.context;

import com.intel.podm.business.services.context.ContextType;
import com.intel.podm.rest.resources.PathParamConstants;
import org.testng.annotations.Test;

import java.util.Collection;
import java.util.Set;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static java.util.Arrays.asList;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ParamNameToContextTypeMapperTest {
    @Test
    public void whenGettingMappedContextTypeForComputerSystemId_shouldReturnCorrectContextType() {
        ParamNameToContextTypeMapper sut = new ParamNameToContextTypeMapper();

        ContextType actualContextType = sut.get("{computerSystemId}");
        assertEquals(actualContextType, COMPUTER_SYSTEM);
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*sdahjdkshadj.*")
    public void whenGettingMappedContextTypeForUnknownParamName_shouldThrow() {
        ParamNameToContextTypeMapper sut = new ParamNameToContextTypeMapper();
        sut.get("sdahjdkshadj");
    }

    @Test
    public void whenCheckingWhetherComputerSystemIdParamIsKnown_shouldReturnTrue() {
        ParamNameToContextTypeMapper sut = new ParamNameToContextTypeMapper();
        assertTrue(sut.contains("{computerSystemId}"));
    }

    @Test
    public void whenCheckingWhetherUnknownParamIsKnown_shouldReturnFalse() {
        ParamNameToContextTypeMapper sut = new ParamNameToContextTypeMapper();
        assertFalse(sut.contains("sdasdsad"));
    }

    @Test
    public void checkWhetherAllContextTypesAreMapped() {
        Set<ContextType> mappedContextTypes =  newHashSet(ParamNameToContextTypeMapper.MAPPING.values());

        Set<ContextType> allContextTypes = newHashSet(asList(ContextType.values()));
        assertEquals(mappedContextTypes, allContextTypes);
    }

    @Test
    public void checkWhetherMappingIsInjective() {
        Collection<ContextType> values = ParamNameToContextTypeMapper.MAPPING.values();

        int numberOfMappedValues = values.size();
        int numberOfDistinctValues = newHashSet(values).size();

        assertEquals(numberOfMappedValues, numberOfDistinctValues);
    }

    @Test
    public void checkWhetherAllParametersAreMapped() {
        Set<String> mappedPathParams = ParamNameToContextTypeMapper.MAPPING.keySet();
        Set<String> allPathParams = newHashSet(PathParamConstants.getPathParameterNames());
        assertEquals(mappedPathParams, allPathParams);
    }
}
