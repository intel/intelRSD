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
import org.springframework.web.client.RestTemplate;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;

import static java.util.UUID.randomUUID;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.ArgumentMatchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.ResponseEntity.ok;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertTrue;

@SuppressWarnings("checkstyle:ExecutableStatementCount")
public class ManagerGetRequestCreatorTest {

    ManagerGetRequestCreator sut;

    @Test
    public void whenGetMethodAndManagerCollectionPathIsGiven_creatorShouldNotBeSelectable() {
        sut = new ManagerGetRequestCreator(null, null);
        assertFalse(sut.canBeSelected(GET, "/redfish/v1/Managers"));
    }

    @Test
    public void whenGetMethodAndManagerInstancePathIsGiven_creatorShouldBeSelectable() {
        sut = new ManagerGetRequestCreator(null, null);
        assertTrue(sut.canBeSelected(GET, "/redfish/v1/Managers/1"));
    }

    @Test(dataProvider = "allHttpMethodsBesideGet")
    public void whenAnyMethodBesideGetAndManagerInstancePathIsGiven_creatorShouldNotBeSelectable(HttpMethod httpMethod) {
        sut = new ManagerGetRequestCreator(null, null);
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Managers/1"));
    }

    @Test
    public void whenThereAreNoResponsesReturnedByUnderlyingLayersAndThereIsNoServiceRegisteredUnderServiceDetector_zeroResponsesShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.process(any())).thenCallRealMethod();
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), any(), any(), any(), any(), any())).thenReturn(new Response[0]);

        val detectedManagersReadonlyCache = mock(DetectedManagersCache.class);

        sut = new ManagerGetRequestCreator(detectedManagersReadonlyCache, defaultReadOnlyRequestCreatorMock);

        val executableRequest = sut.create("/redfish/v1/Managers/1", null, null, null, null);
        val responses = executableRequest.execute(null);

        assertEquals(responses.length, 0);
    }

    @Test
    public void whenThereIsOneResponseReturnedByUnderlyingLayersAndThereIsOneServiceRegisteredUnderServiceDetector_twoResponsesShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.process(any())).thenCallRealMethod();
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), any(), any(), any(), any(), any())).thenReturn(new Response[]{new Response(ok().build())});

        val detectedServicesProviderMock = mock(DetectedManagersCache.class);
        when(detectedServicesProviderMock.get(any())).thenReturn(mock(JsonNode.class));
        when(detectedServicesProviderMock.contains(any())).thenReturn(true);

        val restTemplateMock = mock(RestTemplate.class);
        when(restTemplateMock.exchange(any(), eq(JsonNode.class))).thenReturn(ResponseEntity.ok().build());

        sut = new ManagerGetRequestCreator(detectedServicesProviderMock, defaultReadOnlyRequestCreatorMock);

        val requestedManagerUri = "/redfish/v1/Managers/" + randomUUID().toString();
        val executableRequest = sut.create(requestedManagerUri, null, null, null, null);
        val responses = executableRequest.execute(null);

        assertEquals(responses.length, 2);
        assertEquals(responses[0].getHttpStatus(), OK);
        assertEquals(responses[1].getHttpStatus(), OK);
    }

    @Test
    @SuppressWarnings({"checkstyle:LineLength", "checkstyle:ExecutableStatementCount"})
    public void whenThereAreNoResponsesReturnedByUnderlyingLayersAndThereIsOneServiceRegisteredUnderServiceDetector_oneResponseRepresentingRedfishServiceShouldBeReturned() {
        val defaultReadOnlyRequestCreatorMock = mock(DefaultReadOnlyRequestCreator.class);
        when(defaultReadOnlyRequestCreatorMock.process(any())).thenCallRealMethod();
        when(defaultReadOnlyRequestCreatorMock.invoke(any(), eq(GET), any(), any(), any(), any())).thenReturn(new Response[0]);

        val detectedManagersReadonlyCache = mock(DetectedManagersCache.class);
        when(detectedManagersReadonlyCache.contains(any())).thenReturn(true);
        when(detectedManagersReadonlyCache.get(any())).thenReturn(mock(JsonNode.class));

        val restTemplateMock = mock(RestTemplate.class);
        when(restTemplateMock.exchange(any(), eq(JsonNode.class))).thenReturn(ResponseEntity.ok().body(mock(JsonNode.class)));

        sut = new ManagerGetRequestCreator(detectedManagersReadonlyCache, defaultReadOnlyRequestCreatorMock);

        val uuidOfServiceRegisteredInServiceDetector = randomUUID().toString();
        val requestedManagerUri = "/redfish/v1/Managers/" + uuidOfServiceRegisteredInServiceDetector;
        val responses = sut.create(requestedManagerUri, GET, null, null, null).execute(null);

        assertEquals(responses.length, 1);
        assertEquals(responses[0].getHttpStatus(), OK);
        assertNotNull(responses[0].getBody());
    }

    @DataProvider
    private Object[][] allHttpMethodsBesideGet() {
        return Arrays.stream(HttpMethod.values()).filter(method -> !GET.equals(method)).map(method -> new Object[]{method}).toArray(Object[][]::new);
    }
}
