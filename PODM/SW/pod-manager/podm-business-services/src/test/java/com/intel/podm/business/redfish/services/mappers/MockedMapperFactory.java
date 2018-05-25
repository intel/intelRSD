/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.redfish.services.mappers;

import com.intel.podm.business.redfish.services.helpers.UnknownOemTranslator;

import static java.util.Collections.emptyList;
import static java.util.Optional.of;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public final class MockedMapperFactory {
    private MockedMapperFactory() {
    }

    public static EntityToDtoMapper createManagerMapperWithMockedOem() {
        ManagerDtoMapper managerDtoMapper = new ManagerDtoMapper();
        UnknownOemTranslator mockedTranslator = mock(UnknownOemTranslator.class);
        when(mockedTranslator.translateUnknownOemToDtos(any(), any())).thenReturn(emptyList());
        managerDtoMapper.setUnknownOemTranslator(mockedTranslator);

        MapperProducer mockedMapperProducer = mock(MapperProducer.class);
        when(mockedMapperProducer.tryFindDtoMapperForEntity(any())).thenReturn(of(managerDtoMapper));

        return new EntityToDtoMapper(mockedMapperProducer);
    }

    public static EntityToDtoMapper createMockedEntityToDtoMapper() {
        FabricDtoMapper fabricMapper = new FabricDtoMapper();
        UnknownOemTranslator mockedTranslator = mock(UnknownOemTranslator.class);
        when(mockedTranslator.translateUnknownOemToDtos(any(), any())).thenReturn(emptyList());
        fabricMapper.setUnknownOemTranslator(mockedTranslator);

        MapperProducer mockedMapperProducer = mock(MapperProducer.class);
        when(mockedMapperProducer.tryFindDtoMapperForEntity(any())).thenReturn(of(fabricMapper));

        return new EntityToDtoMapper(mockedMapperProducer);
    }
}
