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

import java.util.Arrays;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.rsd.resourcemanager.layers.merger.request.creators.SelectableRequestCreator.RW_METHODS;
import static java.util.Collections.emptySet;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.ResponseEntity.ok;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class DefaultReadWriteRequestCreatorTest {

    DefaultReadWriteRequestCreator sut;

    @Mock
    private Layer nextLayer;

    @BeforeMethod
    public void setUp() {
        initMocks(this);
        this.sut = new DefaultReadWriteRequestCreator();
    }

    @Test(dataProvider = "allHttpReadWriteMethods")
    public void whenAnyReadWriteMethodIsGiven_creatorShouldBeSelectable(HttpMethod httpMethod) {
        assertTrue(sut.canBeSelected(httpMethod, "/raz/dwa"));
    }

    @Test(dataProvider = "allHttpReadOnlyMethods")
    public void whenReadOnlyMethodIsGiven_creatorsShouldNotBeSelectable(HttpMethod httpMethod) {
        assertFalse(sut.canBeSelected(httpMethod, "/one/two"));
    }

    @Test
    public void whenRequestedPathIsNotRelatedWithAnyUnderlyingService_noResponseToMerge() {
        when(nextLayer.getServicesExposingPath(any())).thenReturn(emptySet());
        Response[] result = sut.create("/redfish/v1/Any/asset", POST, new HttpHeaders(), null, null).execute(nextLayer);
        assertTrue(result.length == 0);
    }

    @Test(expectedExceptions = IllegalStateException.class)
    public void whenRequestedPathIsRelatedWithMoreThanOneUnderlyingServices_exceptionShouldBeThrown() {

        when(nextLayer.getServicesExposingPath(any()))
            .thenReturn(newHashSet(new ServiceId("s1"), new ServiceId("s2")));

        sut.create("/redfish/v1/Any/asset", POST, new HttpHeaders(), null, null).execute(nextLayer);
    }

    @Test
    public void whenRequestedPathIsRelatedWithOneUnderlyingService_oneResponseShouldBeReturned() {
        HttpMethod testedHttpMethod = POST;

        when(nextLayer.getServicesExposingPath(any()))
            .thenReturn(newHashSet(new ServiceId("s1")));

        when(nextLayer.invoke(any(), any(), eq(testedHttpMethod), any(), any(), any()))
            .thenReturn(new Response(ok().build()));

        Response[] result = sut.create("/redfish/v1/Any/asset", testedHttpMethod, new HttpHeaders(), null, null).execute(nextLayer);
        assertTrue(result.length == 1);
    }

    @DataProvider
    private Object[][] allHttpReadWriteMethods() {
        return RW_METHODS.stream().map(method -> new Object[]{method}).toArray(Object[][]::new);
    }

    @DataProvider
    private Object[][] allHttpReadOnlyMethods() {
        return Arrays.stream(HttpMethod.values()).filter(method -> !RW_METHODS.contains(method)).map(method -> new Object[]{method}).toArray(Object[][]::new);
    }
}
