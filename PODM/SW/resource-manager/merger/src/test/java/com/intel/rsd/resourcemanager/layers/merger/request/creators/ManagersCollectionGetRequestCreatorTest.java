/*
 * Copyright (c) 2018-2019 Intel Corporation
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

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.val;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;

import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpMethod.GET;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ManagersCollectionGetRequestCreatorTest {

    @Test
    public void whenMethodGetAndManagerPathIsGiven_creatorsShouldBeSelectable() {
        val sut = new ManagersCollectionGetRequestCreator(null, null);
        assertTrue(sut.canBeSelected(GET, "/redfish/v1/Managers"));
    }

    @Test
    public void whenMethodGetAndManagerInstancePathIsGiven_creatorsShouldNotBeSelectable() {
        val sut = new ManagersCollectionGetRequestCreator(null, null);
        assertFalse(sut.canBeSelected(GET, "/redfish/v1/Managers/1"));
    }

    @Test(dataProvider = "allHttpMethods")
    public void whenNonGetMethodAndManagerCollectionPathIsGiven_creatorsShouldNotBeSelectable(HttpMethod httpMethod) {
        val sut = new ManagersCollectionGetRequestCreator(null, null);

        if (GET.equals(httpMethod)) {
            assertTrue(sut.canBeSelected(httpMethod, "/redfish/v1/Managers"));
        } else {
            assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Managers"));
        }
    }

    @Test(dataProvider = "allHttpMethods")
    public void whenNonGetMethodAndManagerInstancePathIsGiven_creatorsShouldNotBeSelectable(HttpMethod httpMethod) {
        val sut = new ManagersCollectionGetRequestCreator(null, null);
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Managers/1"));
    }

    @Test
    public void whenThereAreNoUnderlyingServicesAndZeroResponsesReturnedByServiceDetector_ZeroResponsesShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), any(), any(), any(), any(), any()))
            .thenReturn(new Response[0]);
        when(defaultReadOnlyRequestCreatorMock.process(any()))
            .thenCallRealMethod();

        val detectedServicesProvider = mock(DetectedManagersCache.class);

        val sut = new ManagersCollectionGetRequestCreator(detectedServicesProvider, defaultReadOnlyRequestCreatorMock);
        val executableRequest = sut.create(null, null, null, null, null);
        val responses = executableRequest.execute(null);

        assertEquals(responses.length, 0);
    }

    @Test
    public void whenThereIsOneResponseReturnedByUnderlyingLayersAndZeroResponsesReturnedByServiceDetector_OneResponsesShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), any(), any(), any(), any(), any()))
            .thenReturn(new Response[]{new Response(ResponseEntity.ok().build())});

        when(defaultReadOnlyRequestCreatorMock.process(any()))
            .thenCallRealMethod();

        val detectedServicesProvider = mock(DetectedManagersCache.class);

        val sut = new ManagersCollectionGetRequestCreator(detectedServicesProvider, defaultReadOnlyRequestCreatorMock);
        val executableRequest = sut.create(null, null, null, null, null);
        val responses = executableRequest.execute(null);

        assertEquals(responses.length, 1);
    }

    @Test
    public void whenThereIsOneResponseReturnedByUnderlyingLayersAndOneResponseReturnedByServiceDetector_TwoResponsesShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), any(), any(), any(), any(), any()))
            .thenReturn(new Response[]{new Response(ResponseEntity.ok().build())});
        when(defaultReadOnlyRequestCreatorMock.process(any()))
            .thenCallRealMethod();

        val detectedManagersReadonlyCache = mock(DetectedManagersCache.class);
        when(detectedManagersReadonlyCache.getCollection()).thenReturn(mock(JsonNode.class));

        val sut = new ManagersCollectionGetRequestCreator(detectedManagersReadonlyCache, defaultReadOnlyRequestCreatorMock);
        val executableRequest = sut.create(null, null, null, null, null);
        val responses = executableRequest.execute(null);

        assertEquals(responses.length, 2);
    }

    @Test
    public void whenThereIsZeroResponsesReturnedByUnderlyingLayersAndOneResponseReturnedByServiceDetector_OneResponsesShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), any(), any(), any(), any(), any()))
            .thenReturn(new Response[0]);

        when(defaultReadOnlyRequestCreatorMock.process(any()))
            .thenCallRealMethod();

        val detectedManagersReadonlyCache = mock(DetectedManagersCache.class);
        when(detectedManagersReadonlyCache.getCollection()).thenReturn(mock(JsonNode.class));

        val sut = new ManagersCollectionGetRequestCreator(detectedManagersReadonlyCache, defaultReadOnlyRequestCreatorMock);
        val executableRequest = sut.create(null, null, null, null, null);
        val responses = executableRequest.execute(null);

        assertEquals(responses.length, 1);
    }

    @DataProvider
    private Object[][] allHttpMethods() {
        return Arrays.stream(HttpMethod.values()).map(method -> new Object[]{method}).toArray(Object[][]::new);
    }

}
