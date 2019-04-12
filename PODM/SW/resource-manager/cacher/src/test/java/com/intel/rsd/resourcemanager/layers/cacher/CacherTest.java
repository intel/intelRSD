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

package com.intel.rsd.resourcemanager.layers.cacher;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.mockito.Mock;
import org.mockito.stubbing.OngoingStubbing;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.HashMap;
import java.util.stream.Stream;

import static java.util.Collections.singleton;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.DELETE;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.HEAD;
import static org.springframework.http.HttpMethod.OPTIONS;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpMethod.PUT;
import static org.springframework.http.HttpMethod.TRACE;
import static org.springframework.http.HttpStatus.BAD_GATEWAY;
import static org.springframework.http.HttpStatus.GATEWAY_TIMEOUT;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.NO_CONTENT;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertTrue;

public class CacherTest {
    private static final Response NOT_FOUND_RESPONSE = new Response(ResponseEntity.notFound().build());
    private static final Response NO_CONTENT_RESPONSE = new Response(null, emptyHeaders(), NO_CONTENT);
    private static final Response INTERNAL_SERVER_ERROR_RESPONSE = new Response(null, emptyHeaders(), INTERNAL_SERVER_ERROR);
    private static final Response SERVICE_UNAVAILABLE_RESPONSE = new Response(null, emptyHeaders(), SERVICE_UNAVAILABLE);
    private static final Response BAD_GATEWAY_RESPONSE = new Response(null, emptyHeaders(), BAD_GATEWAY);
    private static final Response GATEWAY_TIMEOUT_RESPONSE = new Response(null, emptyHeaders(), GATEWAY_TIMEOUT);
    private Cacher sut;

    private Cache cache;

    @Mock
    private Layer nextLayer;

    @BeforeMethod
    void beforeMethod() {
        initMocks(this);
        cache = new SharedCache(new HashMap<>());
        sut = new Cacher(cache);
        sut.setNext(nextLayer);
    }


    @Test(dataProvider = "passThroughMethods")
    public void whenPassThroughMethodIsRequested_thenItShouldOmitCache(HttpMethod passThroughMethod) {
        val anyServiceId = anyServiceId("anyService");
        val path = anyPath();

        cache.updateOrCreate(new ResourceReference(anyServiceId, path), new Response(null, emptyHeaders(), OK));

        whenNextLayerInvokeCalledWithMethod(passThroughMethod).thenReturn(SERVICE_UNAVAILABLE_RESPONSE);

        val result = sut.invoke(anyServiceId, path, passThroughMethod, headersForCached(), null);
        assertResponseIsNotCached(result);
        assertEquals(result.getHttpStatus(), SERVICE_UNAVAILABLE_RESPONSE.getHttpStatus());
        assertEquals(result.getBody(), SERVICE_UNAVAILABLE_RESPONSE.getBody());
    }

    private static HttpHeaders emptyHeaders() {
        return new HttpHeaders();
    }

    @Test
    public void whenUnderlyingServiceReturns404_thenResponseShouldNotBeCached() throws Exception {
        val anyServiceId = anyServiceId("anyService");
        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(NOT_FOUND_RESPONSE);
        val cacheSpy = spy(cache);

        sut.invoke(anyServiceId, anyPath(), GET);

        verify(cacheSpy, never()).updateOrCreate(any(), any());
    }

    @Test
    public void whenInvokedWithMethodOtherThanGet_thenShouldNotCacheResponse() throws Exception {
        val anyServiceId = anyServiceId("anyService");
        val path = anyPath();
        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        val cacheSpy = spy(cache);
        whenNextLayerInvokeCalledWithMethod(POST).thenReturn(new Response(null, emptyHeaders(), OK));

        sut.invoke(anyServiceId, path, POST);

        verify(cacheSpy, never()).updateOrCreate(any(), any());
    }

    @Test
    public void whenInvokedUriIsCachedAndNextLayerReturns503OnPost_thenNextReadOnlyRequestShouldReturnResponseFromCache() {
        val anyServiceId = anyServiceId("any");
        val path = anyPath();
        val response = new Response(null, emptyHeaders(), OK);
        cache.updateOrCreate(new ResourceReference(anyServiceId, path), response);

        //response should be cached
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(SERVICE_UNAVAILABLE_RESPONSE);
        val getRequestResult = sut.invoke(anyServiceId, path, GET, headersForCached(), null);
        assertResponseIsCached(getRequestResult);

        //RW request response should not be cached
        whenNextLayerInvokeCalledWithMethod(POST).thenReturn(SERVICE_UNAVAILABLE_RESPONSE);
        val result = sut.invoke(anyServiceId, path, POST, headersForCached(), null);
        assertResponseIsNotCached(result);
        assertEquals(result.getHttpStatus(), SERVICE_UNAVAILABLE_RESPONSE.getHttpStatus());
        assertEquals(result.getBody(), SERVICE_UNAVAILABLE_RESPONSE.getBody());

        //even after failed RW request, RO response should STILL be cached
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(SERVICE_UNAVAILABLE_RESPONSE);
        val getSecondRequestResult = sut.invoke(anyServiceId, path, GET, headersForCached(), null);
        assertResponseIsCached(getSecondRequestResult);
    }

    @Test
    public void whenInvokedUriIsCachedAndNextLayerReturns503_thenShouldReturnResponseFromCache() throws Exception {
        val anyServiceId = anyServiceId("anyService");
        val anyPath = anyPath();
        val responseBody = JsonNodeFactory.instance.objectNode().put("cached", "response");
        val expectedResponse = new Response(responseBody, emptyHeaders(), OK);
        val resourceReference = new ResourceReference(anyServiceId, anyPath);
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(SERVICE_UNAVAILABLE_RESPONSE);
        cache.updateOrCreate(resourceReference, expectedResponse);
        val response = sut.invoke(anyServiceId, anyPath, GET, headersForCached(), null);

        assertEquals(response.getHttpStatus(), expectedResponse.getHttpStatus());
        assertEquals(response.getBody(), expectedResponse.getBody());
    }

    @Test(dataProvider = "responsesToTest")
    public void whenUnderlyingServiceReturns200AndThenError_thenResponseShouldBeCached(Response errorToTest) throws Exception {
        val path = anyPath();
        val anyServiceId = anyServiceId("anyService");
        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        val responseBody = JsonNodeFactory.instance.objectNode().put("cached", "response");
        val nextLayerResponse = new Response(responseBody, emptyHeaders(), OK);
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(nextLayerResponse);

        sut.invoke(anyServiceId, path, GET);
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(errorToTest);
        val result = sut.invoke(anyServiceId, path, GET);

        assertResponseIsCached(result);
        assertEquals(result.getHttpStatus(), nextLayerResponse.getHttpStatus());
        assertEquals(result.getBody(), nextLayerResponse.getBody());
    }

    @Test(dataProvider = "responsesToTest")
    public void whenResourceIsCachedThenDeletedAndThenNextLayerReturnsError_thenResourceShouldNotBeCached(Response errorToTest) throws Exception {
        val anyServiceId = anyServiceId("any");
        val path = anyPath();
        cache.updateOrCreate(new ResourceReference(anyServiceId, path), new Response(null, emptyHeaders(), OK));

        whenNextLayerInvokeCalledWithMethod(DELETE).thenReturn(NO_CONTENT_RESPONSE);

        sut.invoke(anyServiceId, path, DELETE);

        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(errorToTest);

        val result = sut.invoke(anyServiceId, path, GET);

        assertResponseIsNotCached(result);
        assertEquals(result.getHttpStatus(), errorToTest.getHttpStatus());
        assertEquals(result.getBody(), errorToTest.getBody());
    }

    @Test(dataProvider = "responsesToTest")
    public void whenResourceIsCachedThenPatchedAndThenNextLayerReturnsError_thenResourceShouldNotBeCached(Response errorToTest) throws Exception {
        val anyServiceId = anyServiceId("any");
        val path = anyPath();
        val response = new Response(null, emptyHeaders(), OK);
        cache.updateOrCreate(new ResourceReference(anyServiceId, path), response);

        whenNextLayerInvokeCalledWithMethod(PATCH).thenReturn(NO_CONTENT_RESPONSE);

        sut.invoke(anyServiceId, path, PATCH);

        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(errorToTest);

        val result = sut.invoke(anyServiceId, path, GET);

        assertResponseIsNotCached(result);
        assertEquals(result.getHttpStatus(), errorToTest.getHttpStatus());
        assertEquals(result.getBody(), errorToTest.getBody());
    }

    @Test(dataProvider = "responsesToTest")
    public void whenResourceIsCachedThenPutAndThenNextLayerReturnsError_thenResourceShouldNotBeCached(Response errorToTest) throws Exception {
        val anyServiceId = anyServiceId("any");
        val path = anyPath();
        val response = new Response(null, emptyHeaders(), OK);
        cache.updateOrCreate(new ResourceReference(anyServiceId, path), response);

        whenNextLayerInvokeCalledWithMethod(PUT).thenReturn(NO_CONTENT_RESPONSE);

        sut.invoke(anyServiceId, path, PUT);

        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(errorToTest);

        val result = sut.invoke(anyServiceId, path, GET);

        assertResponseIsNotCached(result);
        assertEquals(result.getHttpStatus(), errorToTest.getHttpStatus());
        assertEquals(result.getBody(), errorToTest.getBody());
    }

    @Test(dataProvider = "responsesToTest")
    public void whenResourceIsCachedThenReturns404AndThenNextLayerReturnsError_thenResourceShouldBeRemovedFromCache(Response errorToTest) throws Exception {
        val anyServiceId = anyServiceId("any");
        val path = anyPath();
        cache.updateOrCreate(new ResourceReference(anyServiceId, path), new Response(null, emptyHeaders(), OK));
        when(nextLayer.getServices()).thenReturn(singleton(anyServiceId));
        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(NOT_FOUND_RESPONSE);

        val getForNotCachedResponse = sut.invoke(anyServiceId, path, GET, headersForNotCached());
        assertEquals(getForNotCachedResponse.getHttpStatus(), NOT_FOUND_RESPONSE.getHttpStatus());

        whenNextLayerInvokeCalledWithMethod(GET).thenReturn(errorToTest);
        val getForCachedResponse = sut.invoke(anyServiceId, path, GET, headersForCached());
        assertResponseIsNotCached(getForCachedResponse);
        assertEquals(getForCachedResponse.getHttpStatus(), errorToTest.getHttpStatus());
        assertEquals(getForCachedResponse.getBody(), errorToTest.getBody());
    }

    @DataProvider
    private Object[][] responsesToTest() {
        return new Object[][]{
            new Object[]{SERVICE_UNAVAILABLE_RESPONSE},
            new Object[]{INTERNAL_SERVER_ERROR_RESPONSE},
            new Object[]{BAD_GATEWAY_RESPONSE},
            new Object[]{GATEWAY_TIMEOUT_RESPONSE}
        };
    }

    @DataProvider
    private Object[][] passThroughMethods() {
        return Stream.of(HEAD, OPTIONS, TRACE).map(response -> new Object[]{response}).toArray(Object[][]::new);
    }

    private OngoingStubbing<Response> whenNextLayerInvokeCalledWithMethod(HttpMethod get) {
        return when(
            nextLayer.invoke(
                any(ServiceId.class),
                any(String.class),
                eq(get),
                any(HttpHeaders.class),
                any(JsonNode.class),
                any()
            )
        );
    }

    private HttpHeaders headersForNotCached() {
        val headers = emptyHeaders();
        headers.setCacheControl("max-stale=0");
        return headers;
    }

    private HttpHeaders headersForCached() {
        val httpHeaders = emptyHeaders();
        httpHeaders.setCacheControl("max-stale=99999");
        return httpHeaders;
    }

    private void assertResponseIsNotCached(Response response) {
        assertNotNull(response);
        assertFalse(response.getHttpHeaders().containsKey("Age"));
    }

    private void assertResponseIsCached(Response response) {
        assertNotNull(response);
        assertTrue(response.getHttpHeaders().containsKey("Age"));
    }

    private ServiceId anyServiceId(String anyService) {
        return new ServiceId(anyService);
    }

    private String anyPath() {
        return "/any/path";
    }
}
