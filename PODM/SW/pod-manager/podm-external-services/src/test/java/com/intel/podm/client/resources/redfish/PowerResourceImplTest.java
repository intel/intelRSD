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
public class PowerResourceImplTest {
    @Test
    public void getByFragment_shouldReturnValidVoltage() throws Exception {
        PowerResourceImpl powerMock = new PowerResourceImpl();
        powerMock.voltages = asList(
                createReferenceable(PowerResourceImpl.Voltage.class, "2"),
                createReferenceable(PowerResourceImpl.Voltage.class, "1")
        );

        PowerResourceImpl.Voltage voltage = (PowerResourceImpl.Voltage) powerMock.getByFragment("/Voltages/1");

        assertTrue(voltage != null);
        assertEquals(voltage.getMemberId(), "1");
    }

    @Test
    public void getByFragment_shouldReturnValidPowerSupply() throws Exception {
        PowerResourceImpl powerMock = new PowerResourceImpl();
        powerMock.powerSupplies = asList(
                createReferenceable(PowerResourceImpl.PowerSupply.class, "2"),
                createReferenceable(PowerResourceImpl.PowerSupply.class, "1")
        );

        PowerResourceImpl.PowerSupply powerSupply =
                (PowerResourceImpl.PowerSupply) powerMock.getByFragment("/PowerSupplies/1");

        assertTrue(powerSupply != null);
        assertEquals(powerSupply.getMemberId(), "1");
    }

    @Test
    public void getByFragment_shouldReturnNull_whenCalledWithInvalidCollectionName() throws Exception {
        PowerResourceImpl powerMock = new PowerResourceImpl();
        PowerResourceImpl resource = (PowerResourceImpl) powerMock.getByFragment("/Unknown/1");
        assertEquals(resource, null);
    }

    private <T extends ReferenceableMember> T createReferenceable(Class<T> clazz, String memberId) {
        T object = mock(clazz);
        when(object.getMemberId()).thenReturn(memberId);
        return object;
    }
}
