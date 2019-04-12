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
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.common.DetectedManagersCache;
import com.intel.rsd.resourcemanager.common.ServiceDetectorApiConsumer;
import lombok.val;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Arrays;
import java.util.stream.Stream;

import static java.util.UUID.randomUUID;
import static org.mockito.ArgumentMatchers.any;
import static org.mockito.Mockito.doNothing;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;
import static org.springframework.http.ResponseEntity.badRequest;
import static org.springframework.http.ResponseEntity.notFound;
import static org.springframework.http.ResponseEntity.ok;
import static org.springframework.http.ResponseEntity.status;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ManagersCollectionPostRequestCreatorTest {

    ManagersCollectionPostRequestCreator sut;

    @Test
    public void whenPostMethodAndManagersCollectionPathIsGiven_creatorShouldBeSelectable() {
        sut = new ManagersCollectionPostRequestCreator(null, null, null);
        assertTrue(sut.canBeSelected(POST, "/redfish/v1/Managers"));
    }

    @Test
    public void whenPostMethodAndManagerInstancePathIsGiven_creatorShouldNotBeSelectable() {
        sut = new ManagersCollectionPostRequestCreator(null, null, null);
        assertFalse(sut.canBeSelected(POST, "/redfish/v1/Managers/1"));
    }

    @Test(dataProvider = "allHttpMethodsBesidePost")
    public void whenNonPostMethodAndManagerCollectionPathIsGiven_creatorShouldNotBeSelectable(HttpMethod httpMethod) {
        sut = new ManagersCollectionPostRequestCreator(null, null, null);
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Managers"));
    }

    @Test(dataProvider = "allHttpMethodsBesidePost")
    public void whenNonPostMethodAndManagerInstancePathIsGiven_creatorShouldNotBeSelectable(HttpMethod httpMethod) {
        val sut = new ManagersCollectionPostRequestCreator(null, null, null);
        assertFalse(sut.canBeSelected(httpMethod, "/redfish/v1/Managers/1"));
    }

    @Test(dataProvider = "errorResponses")
    public void whenServiceDetectorReturnsNon2xx_sameResponseShouldBeReturned(ResponseEntity<JsonNode> errorResponse) {

        val serviceDetectorApiConsumer = mock(ServiceDetectorApiConsumer.class);
        when(serviceDetectorApiConsumer.registerManager(any())).thenReturn(errorResponse);

        val detectedManagersCache = mock(DetectedManagersCache.class);

        sut = new ManagersCollectionPostRequestCreator(serviceDetectorApiConsumer, detectedManagersCache, null);
        val executableRequest = sut.create(null, null, null, JsonNodeFactory.instance.objectNode(), null);
        val result = executableRequest.execute(null);

        assertEquals(1, result.length);
        assertEquals(result[0].getHttpStatus(), errorResponse.getStatusCode());
    }

    @Test
    public void whenServiceDetectorReturnsNon2xxResponseThen_serviceDetectorResponseShouldBeReturned() {
        val serviceDetectorApiConsumer = mock(ServiceDetectorApiConsumer.class);
        when(serviceDetectorApiConsumer.registerManager(any())).thenReturn(badRequest().build());

        val detectedManagersWritableCache = mock(DetectedManagersCache.class);

        sut = new ManagersCollectionPostRequestCreator(serviceDetectorApiConsumer, detectedManagersWritableCache, null);

        val requestBody = createRequestBody("localhost:9999", randomUUID().toString());

        val executableRequest = sut.create(null, null, null, requestBody, null);
        val result = executableRequest.execute(null);

        assertEquals(1, result.length);
        assertEquals(result[0].getHttpStatus(), BAD_REQUEST);
    }

    @Test
    public void whenCorrectRequestIsReceived_201ShouldBeReturned() {
        val serviceDetectorApiConsumer = mock(ServiceDetectorApiConsumer.class);
        when(serviceDetectorApiConsumer.registerManager(any())).thenReturn(ok().build());
        when(serviceDetectorApiConsumer.getExpandedManagers()).thenReturn(ok().build());

        val detectedManagersWritableCache = mock(DetectedManagersCache.class);
        doNothing().when(detectedManagersWritableCache).update(any());

        sut = new ManagersCollectionPostRequestCreator(serviceDetectorApiConsumer, detectedManagersWritableCache, null);

        val requestedUuid = randomUUID().toString();
        val requestedUri = "localhost:9999";

        val requestBody = createRequestBody(requestedUri, requestedUuid);

        val executableRequest = sut.create(null, null, null, requestBody, null);
        val result = executableRequest.execute(null);

        assertEquals(1, result.length);
        assertEquals(result[0].getHttpStatus(), CREATED);
    }

    private ObjectNode createRequestBody(String uri, String uuid) {
        return JsonNodeFactory.instance.objectNode()
            .put("RemoteRedfishServiceUri", uri)
            .put("ServiceEntryPointUUID", uuid);
    }

    @DataProvider
    private Object[][] allHttpMethodsBesidePost() {
        return Arrays.stream(HttpMethod.values()).filter(method -> !POST.equals(method)).map(method -> new Object[]{method}).toArray(Object[][]::new);
    }

    @DataProvider
    private Object[][] errorResponses() {
        return Stream.of(badRequest().build(), notFound().build(), status(INTERNAL_SERVER_ERROR).build(), status(SERVICE_UNAVAILABLE).build())
            .map(response -> new Object[]{response})
            .toArray(Object[][]::new);
    }
}
