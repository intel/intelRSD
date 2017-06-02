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

import com.intel.podm.business.entities.redfish.Processor;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.services.context.ContextType;
import org.testng.annotations.Test;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import static com.google.common.collect.Sets.symmetricDifference;
import static com.intel.podm.business.services.context.ContextType.EVENT_SERVICE;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static java.util.Arrays.asList;
import static java.util.Collections.emptySet;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName"})
public class ContextTypeToEntityMapperTest {
    @Test
    public void whenGettingMappedClassForProcessorContextType_ShouldReturnCorrectClass() {
        ContextTypeToEntityMapper sut = new ContextTypeToEntityMapper();
        assertEquals(sut.get(PROCESSOR), Processor.class);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenGettingMappedClassForNullContextType_shouldThrow() {
        ContextTypeToEntityMapper sut = new ContextTypeToEntityMapper();
        sut.get(null);
    }

    @Test
    public void checkWhetherAllContextTypesAreMapped() {
        Set<ContextType> mappedContextTypes = ContextTypeToEntityMapper.MAPPING.keySet();

        Set<ContextType> allContextTypes = new HashSet<>(asList(ContextType.values()));

        excludeTopLevelSingletonServices(allContextTypes);

        assertEquals(symmetricDifference(mappedContextTypes, allContextTypes), emptySet());
    }

    private void excludeTopLevelSingletonServices(Set<ContextType> allContextTypes) {
        allContextTypes.remove(EVENT_SERVICE);
    }

    @Test
    public void checkWhetherMappingIsInjective() {
        Collection<Class<? extends Entity>> values = ContextTypeToEntityMapper.MAPPING.values();

        int numberOfMappedValues = values.size();
        int numberOfDistinctValues = new HashSet<>(values).size();

        assertEquals(numberOfMappedValues, numberOfDistinctValues);
    }
}
