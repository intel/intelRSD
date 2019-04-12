/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.rsd.resourcemanager.layers.merger.request.creators.SelectableRequestCreator.RW_METHODS;
import static java.util.Collections.emptySet;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.ResponseEntity.ok;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ComputerSystemReadWriteRequestCreatorTest {

    private static final String COMPUTER_SYSTEM_URI = "/redfish/v1/Systems/pararara";

    ComputerSystemReadWriteRequestCreator sut;

    @Mock
    private Layer nextLayer;

    @BeforeMethod
    public void setUp() throws Exception {
        initMocks(this);
        this.sut = new ComputerSystemReadWriteRequestCreator();
    }

    @Test
    public void whenRequestedActionIsRelatedWithPhysicalSystemOnly_responseFromRelatedServiceShouldBeReturned() throws Exception {
        ServiceId serviceId = new ServiceId("1");

        when(nextLayer.getServicesExposingPath(any()))
            .thenReturn(newHashSet(serviceId));

        when(nextLayer.invoke(eq(serviceId), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode().put("SystemType", "Physical"), new HttpHeaders(), OK));

        when(nextLayer.invoke(eq(serviceId), eq(COMPUTER_SYSTEM_URI), eq(PATCH), any(), any()))
            .thenReturn(new Response(ok().build()));

        Response[] result = sut.create(COMPUTER_SYSTEM_URI, PATCH, new HttpHeaders(), null, null).execute(nextLayer);

        assertTrue(result.length == 1);
    }

    @Test
    public void whenRequestedActionIsRelatedWithTwoPhysicalSystems_responsesFromTwoServicesShouldBeReturned() throws Exception {
        ServiceId serviceId1 = new ServiceId("1");
        ServiceId serviceId2 = new ServiceId("2");

        when(nextLayer.getServicesExposingPath(any()))
            .thenReturn(newHashSet(serviceId1, serviceId2));

        when(nextLayer.invoke(eq(serviceId1), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode().put("SystemType", "Physical"), new HttpHeaders(), OK));

        when(nextLayer.invoke(eq(serviceId2), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode().put("SystemType", "Physical"), new HttpHeaders(), OK));

        when(nextLayer.invoke(eq(serviceId1), eq(COMPUTER_SYSTEM_URI), eq(PATCH), any(), any()))
            .thenReturn(new Response(ok().build()));

        when(nextLayer.invoke(eq(serviceId2), eq(COMPUTER_SYSTEM_URI), eq(PATCH), any(), any()))
            .thenReturn(new Response(ok().build()));

        Response[] result = sut.create(COMPUTER_SYSTEM_URI, PATCH, new HttpHeaders(), null, null).execute(nextLayer);

        assertTrue(result.length == 2);
    }

    @Test
    public void whenRequestedActionIsRelatedWithPhysicalAndVirtualSystems_responseFromServiceExposingPhysicalSystemShouldBeReturned() throws Exception {
        ServiceId serviceWithPhysicalSystem = new ServiceId("1");
        ServiceId serviceWithVirtualSystem = new ServiceId("2");

        when(nextLayer.getServicesExposingPath(any())).thenReturn(newHashSet(serviceWithPhysicalSystem, serviceWithVirtualSystem));

        when(nextLayer.invoke(eq(serviceWithPhysicalSystem), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode().put("SystemType", "Physical"), new HttpHeaders(), OK));

        when(nextLayer.invoke(eq(serviceWithVirtualSystem), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode().put("SystemType", "Virtual"), new HttpHeaders(), OK));

        when(nextLayer.invoke(eq(serviceWithPhysicalSystem), eq(COMPUTER_SYSTEM_URI), eq(PATCH), any(), any()))
            .thenReturn(new Response(ok().build()));

        Response[] result = sut.create(COMPUTER_SYSTEM_URI, PATCH, new HttpHeaders(), null, null).execute(nextLayer);

        assertTrue(result.length == 1);
        verify(nextLayer).invoke(eq(serviceWithPhysicalSystem), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any());
        verify(nextLayer).invoke(eq(serviceWithVirtualSystem), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any());
        verify(nextLayer).invoke(eq(serviceWithPhysicalSystem), any(), eq(PATCH), any(), any(), any());
    }

    @Test
    public void whenRequestedActionIsRelatedOnlyWithVirtualSystem_errorResponseFromServiceExposingVirtualSystemShouldBeReturned() throws Exception {
        ServiceId serviceWithVirtualSystem = new ServiceId("2");
        when(nextLayer.getServicesExposingPath(any())).thenReturn(newHashSet(serviceWithVirtualSystem));

        when(nextLayer.invoke(eq(serviceWithVirtualSystem), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode().put("SystemType", "Virtual"), new HttpHeaders(), OK));

        Response[] result = sut.create(COMPUTER_SYSTEM_URI, PATCH, new HttpHeaders(), null, null).execute(nextLayer);

        assertEquals(result[0].getHttpStatus(), METHOD_NOT_ALLOWED);
    }

    @Test
    public void whenRequestedActionIsRelatedOnlyWithUnknownSystem_errorResponseFromServiceExposingUnknownSystemShouldBeReturned() throws Exception {
        ServiceId serviceSystemWithoutSystemType = new ServiceId("1");
        when(nextLayer.getServicesExposingPath(any())).thenReturn(newHashSet(serviceSystemWithoutSystemType));

        when(nextLayer.invoke(eq(serviceSystemWithoutSystemType), eq(COMPUTER_SYSTEM_URI), eq(GET), any(), any()))
            .thenReturn(new Response(JsonNodeFactory.instance.objectNode(), new HttpHeaders(), OK));

        Response[] result = sut.create(COMPUTER_SYSTEM_URI, PATCH, new HttpHeaders(), null, null).execute(nextLayer);

        assertEquals(result[0].getHttpStatus(), METHOD_NOT_ALLOWED);
    }

    @Test
    public void whenThereIsNoRelatedUnderlyingService_zeroResponsesShouldBeReturned() throws Exception {
        when(nextLayer.getServicesExposingPath(any())).thenReturn(emptySet());
        Response[] result = sut.create(COMPUTER_SYSTEM_URI, POST, new HttpHeaders(), null, null).execute(nextLayer);
        assertTrue(result.length == 0);
    }

    @Test(dataProvider = "allRwMethods")
    public void canBeSelectedShouldReturnTrueIfSystemPathIsProvided(HttpMethod httpMethod) throws Exception {
        assertTrue(sut.canBeSelected(httpMethod, COMPUTER_SYSTEM_URI));
    }

    @Test(dataProvider = "allRwMethods")
    public void canBeSelectedShouldReturnFalseIfSystemActionPathIsProvided(HttpMethod httpMethod) throws Exception {
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Systems/pararara/Actions/ComputerSystem.Reset"));
    }

    @DataProvider
    Object[][] allRwMethods() {
        return Arrays.stream(RW_METHODS.toArray()).map(status -> new Object[]{status}).toArray(Object[][]::new);
    }
}
