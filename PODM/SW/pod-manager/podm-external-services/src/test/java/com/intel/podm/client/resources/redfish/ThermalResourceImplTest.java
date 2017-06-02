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

package com.intel.podm.client.resources.redfish;

import com.intel.podm.client.api.ReferenceableMember;
import org.testng.annotations.Test;

import static java.util.Arrays.asList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class ThermalResourceImplTest {
    @Test
    public void getByFragment_shouldReturnValidRedundancy() throws Exception {
        ThermalResourceImpl thermalMock = new ThermalResourceImpl();
        thermalMock.redundancies = asList(
                createReferenceable(RedundancyObjectImpl.class, "1"),
                createReferenceable(RedundancyObjectImpl.class, "2")
        );

        RedundancyObjectImpl redundancy = (RedundancyObjectImpl) thermalMock.getByFragment("/Redundancy/2");

        assertTrue(redundancy != null);
        assertEquals(redundancy.getMemberId(), "2");
    }

    @Test
    public void getByFragment_shouldReturnValidFan() throws Exception {
        ThermalResourceImpl thermalMock = new ThermalResourceImpl();
        thermalMock.fans = asList(
                createReferenceable(ThermalResourceImpl.Fan.class, "2"),
                createReferenceable(ThermalResourceImpl.Fan.class, "1")
        );

        ThermalResourceImpl.Fan fan = (ThermalResourceImpl.Fan) thermalMock.getByFragment("/Fans/1");

        assertTrue(fan != null);
        assertEquals(fan.getMemberId(), "1");
    }

    @Test
    public void getByFragment_shouldReturnValidTemperature() throws Exception {
        ThermalResourceImpl thermalMock = new ThermalResourceImpl();
        thermalMock.temperatures = asList(
                createReferenceable(ThermalResourceImpl.Temperature.class, "2"),
                createReferenceable(ThermalResourceImpl.Temperature.class, "1")
        );

        ThermalResourceImpl.Temperature temperature =
                (ThermalResourceImpl.Temperature) thermalMock.getByFragment("/Temperatures/1");

        assertTrue(temperature != null);
        assertEquals(temperature.getMemberId(), "1");
    }

    @Test
    public void getByFragment_shouldReturnNull_whenCalledWithInvalidCollectionName() throws Exception {
        ThermalResourceImpl thermalMock = new ThermalResourceImpl();
        ThermalResourceImpl resource = (ThermalResourceImpl) thermalMock.getByFragment("/Intel/1");
        assertEquals(resource, null);
    }

    private <T extends ReferenceableMember> T createReferenceable(Class<T> clazz, String memberId) {
        T object = mock(clazz);
        when(object.getMemberId()).thenReturn(memberId);
        return object;
    }

}
