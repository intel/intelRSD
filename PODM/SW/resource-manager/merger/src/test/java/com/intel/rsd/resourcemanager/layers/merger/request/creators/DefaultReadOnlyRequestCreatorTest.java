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
import com.intel.rsd.resourcemanager.layers.merger.ResourceManagerResourcesProvider;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;

import static com.google.common.collect.Sets.newHashSet;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.ResponseEntity.ok;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class DefaultReadOnlyRequestCreatorTest {

    public static final Response NOT_FOUND_RESPONSE = new Response(ResponseEntity.notFound().build());

    DefaultReadOnlyRequestCreator sut;

    @Mock
    private Layer nextLayer;
    @Mock
    private ResourceManagerResourcesProvider resourceManagerResourcesProvider;

    @BeforeMethod
    public void setUp() throws Exception {
        MockitoAnnotations.initMocks(this);
        this.sut = new DefaultReadOnlyRequestCreator(resourceManagerResourcesProvider, 1);
    }

    @Test(dataProvider = "allMethodsBesideGet")
    public void whenAnyMethodBesideIsGiven_creatorShouldNotBeSelectable(HttpMethod httpMethod) {
        assertFalse(sut.canBeSelected(httpMethod, "/raz/dwa"));
    }

    @Test
    public void whenGetMethodIsGiven_creatorsShouldBeSelectable() {
        assertTrue(sut.canBeSelected(GET, "/one/two"));
    }

    @Test
    public void whenThereAreNoUnderlyingServicesThenThereAreNoResponsesToMerge() throws Exception {
        when(nextLayer.getServices()).thenReturn(newHashSet());
        Response[] result = sut.create("/redfish/v1/unknownResource", GET, new HttpHeaders(), null, null).execute(nextLayer);
        assertTrue(result.length == 0);
    }

    @Test
    public void whenAllUnderlyingServicesReturnNotFoundThenThereAreNoResponsesToMerge() throws Exception {
        when(nextLayer.getServices()).thenReturn(newHashSet(new ServiceId("s1"), new ServiceId("s2")));
        when(nextLayer.invoke(any(), any(), any(), any(), any())).thenReturn(NOT_FOUND_RESPONSE);
        Response[] result = sut.create("/redfish/v1/unknownResource", GET, new HttpHeaders(), null, null).execute(nextLayer);
        assertTrue(result.length == 0);
    }

    @Test
    public void whenUnderlyingServiceReturnsOk_returnedResponseShouldBePassedToResponseMerger() {
        when(nextLayer.getServicesExposingPath(any())).thenReturn(newHashSet(new ServiceId("s1")));
        Response response = new Response(ok(JsonNodeFactory.instance.objectNode()));
        when(nextLayer.invoke(any(), any(), any(), any(), any(), any()))
            .thenReturn(NOT_FOUND_RESPONSE)
            .thenReturn(response);

        Response[] result = sut.create("/redfish/v1/unknownResource", GET, new HttpHeaders(), null, null).execute(nextLayer);
        assertTrue(result.length == 1);
    }

    @DataProvider
    private Object[][] allMethodsBesideGet() {
        return Arrays.stream(HttpMethod.values()).filter(m -> !m.equals(GET)).map(method -> new Object[]{method}).toArray(Object[][]::new);
    }
}
