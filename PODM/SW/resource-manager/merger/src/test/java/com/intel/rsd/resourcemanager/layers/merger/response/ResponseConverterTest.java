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

package com.intel.rsd.resourcemanager.layers.merger.response;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.ResourceMerger;
import lombok.val;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static com.intel.rsd.resourcemanager.layers.merger.response.ErrorResponseFactory.createErrorResponse;
import static java.util.Arrays.stream;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.ResponseEntity.accepted;
import static org.springframework.http.ResponseEntity.notFound;
import static org.springframework.http.ResponseEntity.ok;
import static org.springframework.http.ResponseEntity.status;
import static org.testng.Assert.assertEquals;

public class ResponseConverterTest {

    ResponseConverter sut;

    @Mock
    ResourceMerger resourceMerger;

    @BeforeMethod
    public void setUp() throws Exception {
        initMocks(this);
        this.sut = new ResponseConverter(resourceMerger);
    }

    @Test
    public void whenThereIsNoResponseProvided_notFoundResponseShouldBeReturned() throws Exception {
        val expectedResponse = createErrorResponse(createRedfishError(NOT_FOUND));
        val result = sut.merge();
        assertEquals(result.getBody(), expectedResponse.getBody());
        assertEquals(result.getHttpHeaders(), expectedResponse.getHttpHeaders());
        assertEquals(result.getHttpStatus(), expectedResponse.getHttpStatus());
    }


    @Test
    public void whenOneResponseWithoutBodyIsProvided_itShouldBeReturned() {
        Response response = new Response(ok().build());
        when(resourceMerger.merge(any())).thenReturn(response.getBody());
        Response result = sut.merge(response);
        assertEquals(result.getBody(), response.getBody());
        assertEquals(result.getHttpHeaders(), response.getHttpHeaders());
        assertEquals(result.getHttpStatus(), response.getHttpStatus());
    }

    @Test(dataProvider = "httpStatuses")
    public void whenExactlyOneResponseIsProvided_itShouldBeReturned(HttpStatus httpStatus) {
        Response expectedResponse = new Response(status(httpStatus).body(mock(JsonNode.class)));
        when(resourceMerger.merge(any())).thenReturn(expectedResponse.getBody());
        Response result = sut.merge(expectedResponse);
        assertEquals(result.getBody(), expectedResponse.getBody());
        assertEquals(result.getHttpHeaders(), expectedResponse.getHttpHeaders());
        assertEquals(result.getHttpStatus(), expectedResponse.getHttpStatus());
    }

    @DataProvider
    private Object[][] httpStatuses() {
        return stream(HttpStatus.values()).map(status -> new Object[]{status}).toArray(Object[][]::new);
    }

    @Test
    public void whenOneOfGivenResponsesHasNon2xxStatus_responseWith500ShouldBeReturned() {
        Response response = sut.merge(new Response(ok().build()), new Response(notFound().build()));
        assertEquals(response.getHttpStatus(), INTERNAL_SERVER_ERROR);
    }

    @Test
    public void whenGivenResponsesHaveDifferent2xxStatusCodes_responseWith500ShouldBeReturned() {
        Response response = sut.merge(new Response(ok().build()), new Response(accepted().build()));
        assertEquals(response.getHttpStatus(), INTERNAL_SERVER_ERROR);
    }

    @Test
    public void whenUnderlyingServicesReturned4xxResponses_responseWith4xxShouldBeReturned() {
        Response response = sut.merge(new Response(notFound().build()), new Response(notFound().build()));
        assertEquals(response.getHttpStatus(), NOT_FOUND);
    }

    @Test
    public void whenUnderlyingServicesReturnedCorrectResponses_responseWith200StatusIsExpected() {
        HttpHeaders responseHeaders1 = new HttpHeaders() {{
            add("anyKey", "anyValue1");
        }};

        HttpHeaders responseHeaders2 = new HttpHeaders() {{
            add("anyKey", "anyValue2");
        }};

        HttpHeaders expectedHeaders = new HttpHeaders() {{
            add("anyKey", "anyValue1");
            add("anyKey", "anyValue2");
        }};

        Response response = sut.merge(
            new Response(ok().headers(responseHeaders1).body(mock(JsonNode.class))),
            new Response(ok().headers(responseHeaders2).body(mock(JsonNode.class)))
        );

        assertEquals(response.getHttpStatus(), OK);
        assertEquals(response.getHttpHeaders(), expectedHeaders);
    }
}
