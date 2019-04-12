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

import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.rsd.resourcemanager.layers.merger.request.creators.SelectableRequestCreator.RW_METHODS;
import static java.util.Collections.emptySet;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.ResponseEntity.notFound;
import static org.springframework.http.ResponseEntity.ok;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ChassisReadWriteRequestCreatorTest {

    ChassisReadWriteRequestCreator sut;

    @Mock
    private Layer nextLayer;

    @BeforeMethod
    void beforeMethod() {
        initMocks(this);
        this.sut = new ChassisReadWriteRequestCreator();
    }

    @Test(dataProvider = "allRwMethods")
    public void whenReadWriteMethodAndChassisInstancePathIsGiven_creatorShouldBeSelectable(HttpMethod httpMethod) {
        assertTrue(sut.canBeSelected(httpMethod, "/redfish/v1/Chassis/1"));
    }

    @Test(dataProvider = "allRwMethods")
    public void whenReadWriteMethodAndChassisCollectionPathIsGiven_creatorsShouldNotBeSelectable(HttpMethod httpMethod) {
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Chassis"));
    }

    @Test
    public void whenNonReadWriteMethodAndChassisInstancePathIsGiven_creatorsShouldNotBeSelectable() {
        assertFalse(sut.canBeSelected(GET, "/redfish/v1/Managers"));
    }

    @Test
    public void whenThereIsNoUnderlyingServiceRelatedWithRequestedPath_zeroResponsesShouldBeReturned() {
        when(nextLayer.getServicesExposingPath(any())).thenReturn(emptySet());
        Response[] result = sut.create("/redfish/v1/Chassis/anyChassisId", POST, new HttpHeaders(), null, null).execute(nextLayer);
        verify(nextLayer, never()).invoke(any(), any(), any(), any(), any());
        assertTrue(result.length == 0);
    }

    @Test
    public void whenThereAreTwoUnderlyingServicesRelatedWithRequestedPath_twoResponsesShouldBeReturned() {
        ServiceId s1 = new ServiceId("s1");
        ServiceId s2 = new ServiceId("s2");
        String path = "/any/path";
        Response okResponse = new Response(ok().build());
        Response notFoundResponse = new Response(notFound().build());

        when(nextLayer.getServicesExposingPath(any())).thenReturn(newHashSet(s1, s2));
        when(nextLayer.invoke(any(), any(), any(), any(), any()))
            .thenReturn(okResponse)
            .thenReturn(notFoundResponse);

        Response[] result = sut.create(path, POST, new HttpHeaders(), null, null).execute(nextLayer);
        verify(nextLayer).invoke(eq(s1), eq(path), any(), any(), any(), any());
        verify(nextLayer).invoke(eq(s2), eq(path), any(), any(), any(), any());
        assertTrue(result.length == 2);
    }

    @Test
    public void whenThereIsOneUnderlyingService_thenNextLayerResponseShouldBeDelegatedToResponseConverter() {
        ServiceId s1 = new ServiceId("s1");
        Response okResponse = new Response(ok().build());
        String path = "/any/path";

        when(nextLayer.getServicesExposingPath(any())).thenReturn(newHashSet(s1));
        when(nextLayer.invoke(any(), any(), any(), any(), any())).thenReturn(okResponse);

        Response[] result = sut.create(path, POST, new HttpHeaders(), null, null).execute(nextLayer);
        verify(nextLayer).invoke(eq(s1), eq(path), any(), any(), any(), any());
        assertTrue(result.length == 1);
    }

    @DataProvider
    private Object[][] allRwMethods() {
        return RW_METHODS.stream().map(method -> new Object[]{method}).toArray(Object[][]::new);
    }
}
