/*
 * Copyright (c) 2016-2018 Intel Corporation
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

import com.intel.podm.client.ReferenceableMember;
import org.testng.annotations.Test;

import java.net.URI;

import static java.util.Arrays.asList;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

@SuppressWarnings({"checkstyle:MethodName"})
public class PowerResourceTest {
    @Test
    public void getByFragment_shouldReturnValidVoltage() throws Exception {
        PowerResource powerMock = new PowerResource();
        powerMock.voltages = asList(
            createReferenceable(VoltageItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Power#/Voltages/2")),
            createReferenceable(VoltageItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Power#/Voltages/1"))
        );

        VoltageItem voltage = (VoltageItem) powerMock.getByFragment("/Voltages/1");

        assertTrue(voltage != null);
        assertEquals(voltage.getIdFromUriFragment(voltage.getUri().getFragment()), "1");
    }

    @Test
    public void getByFragment_shouldReturnValidPowerSupply() throws Exception {
        PowerResource powerMock = new PowerResource();
        powerMock.powerSupplies = asList(
            createReferenceable(PowerSupplyItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Power#/PowerSupplies/2")),
            createReferenceable(PowerSupplyItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Power#/PowerSupplies/1"))
        );

        PowerSupplyItem powerSupply =
            (PowerSupplyItem) powerMock.getByFragment("/PowerSupplies/1");

        assertTrue(powerSupply != null);
        assertEquals(powerSupply.getIdFromUriFragment(powerSupply.getUri().getFragment()), "1");
    }

    @Test
    public void getByFragment_shouldReturnNull_whenCalledWithInvalidCollectionName() throws Exception {
        PowerResource powerMock = new PowerResource();
        PowerResource resource = (PowerResource) powerMock.getByFragment("/Unknown/1");
        assertEquals(resource, null);
    }

    private <T extends ReferenceableMember> T createReferenceable(Class<T> clazz, URI uri) {
        T object = mock(clazz);
        when(object.getUri()).thenReturn(uri);
        when(object.getIdFromUriFragment(uri.getFragment())).thenCallRealMethod();
        return object;
    }
}
