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
public class ThermalResourceTest {
    @Test
    public void getByFragment_shouldReturnValidRedundancy() throws Exception {
        ThermalResource thermalMock = new ThermalResource();
        thermalMock.redundancies = asList(
            createReferenceable(RedundancyItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Thermal#/Redundancy/1")),
            createReferenceable(RedundancyItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Thermal#/Redundancy/2"))
        );

        RedundancyItem redundancy = (RedundancyItem) thermalMock.getByFragment("/Redundancy/2");

        assertTrue(redundancy != null);
        assertEquals(redundancy.getIdFromUriFragment(redundancy.getUri().getFragment()), "2");
    }

    @Test
    public void getByFragment_shouldReturnValidFan() throws Exception {
        ThermalResource thermalMock = new ThermalResource();
        thermalMock.fans = asList(
            createReferenceable(ThermalFanItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Thermal#/Fans/2")),
            createReferenceable(ThermalFanItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Thermal#/Fans/1"))
        );

        ThermalFanItem fan = (ThermalFanItem) thermalMock.getByFragment("/Fans/1");

        assertTrue(fan != null);
        assertEquals(fan.getIdFromUriFragment(fan.getUri().getFragment()), "1");
    }

    @Test
    public void getByFragment_shouldReturnValidTemperature() throws Exception {
        ThermalResource thermalMock = new ThermalResource();
        thermalMock.temperatures = asList(
            createReferenceable(TemperatureItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Thermal#/Temperatures/2")),
            createReferenceable(TemperatureItem.class,
                URI.create("http://127.0.0.1:7812/redfish/v1/Chassis/Rack1/Thermal#/Temperatures/1"))
        );

        TemperatureItem temperature =
            (TemperatureItem) thermalMock.getByFragment("/Temperatures/1");

        assertTrue(temperature != null);
        assertEquals(temperature.getIdFromUriFragment(temperature.getUri().getFragment()), "1");
    }

    @Test
    public void getByFragment_shouldReturnNull_whenCalledWithInvalidCollectionName() throws Exception {
        ThermalResource thermalMock = new ThermalResource();
        ThermalResource resource = (ThermalResource) thermalMock.getByFragment("/Intel/1");
        assertEquals(resource, null);
    }

    private <T extends ReferenceableMember> T createReferenceable(Class<T> clazz, URI uri) {
        T object = mock(clazz);
        when(object.getUri()).thenReturn(uri);
        when(object.getIdFromUriFragment(uri.getFragment())).thenCallRealMethod();
        return object;
    }
}
