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

package com.intel.rsd.resourcemanager.runner.requiredlayer;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.common.SouthboundConfig;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import org.mockito.ArgumentMatcher;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.google.common.collect.Sets.newHashSet;
import static java.util.Collections.singletonList;
import static org.assertj.core.api.Assertions.assertThat;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpStatus.OK;

public class RequiredLayerTest {
    @Mock
    RestClient restClient;

    @Mock
    RequiredServiceFinder finder;

    private RequiredLayer sut;

    @BeforeMethod
    void init() {
        initMocks(this);
        sut = new RequiredLayer(restClient, finder, new SouthboundConfig());

        when(finder.findServiceHost(any(ServiceId.class)))
            .thenReturn(URI.create("http://any-service/"));
    }

    @Test
    public void returnsResponseFromUnderlyingHttpClient() throws Exception {
        Response expectedResponse = new Response(null, new HttpHeaders(), OK);

        when(restClient.call(any(URI.class), eq(GET), any(HttpHeaders.class), eq(null)))
            .thenReturn(expectedResponse);

        ServiceId serviceId = new ServiceId("1");
        Response actualResponse = sut.invoke(serviceId, "/redfish/v1/Chassis", GET);

        assertThat(actualResponse.getBody()).isEqualTo(expectedResponse.getBody());
        assertThat(actualResponse.getHttpStatus()).isEqualTo(expectedResponse.getHttpStatus());
        assertThat(actualResponse.getHttpHeaders()).isEqualTo(expectedResponse.getHttpHeaders());
    }

    @Test
    public void sendsRequestWthCorrectUri() throws Exception {
        ServiceId serviceId = new ServiceId("my-service-id");

        when(finder.findServiceHost(serviceId))
            .thenReturn(URI.create("http://service1/"));

        when(restClient.call(any(URI.class), any(HttpMethod.class), any(HttpHeaders.class), any(JsonNode.class)))
            .thenReturn(new Response(null, new HttpHeaders(), OK));

        sut.invoke(serviceId, "/redfish/v1/Managers", GET);

        verify(restClient).call(
            eq(URI.create("http://service1/redfish/v1/Managers")),
            any(HttpMethod.class),
            any(HttpHeaders.class),
            any(JsonNode.class)
        );
    }

    @Test
    public void sendsRequestBody() throws Exception {
        ServiceId serviceId = new ServiceId("my-service-id");

        JsonNode requestBody = mock(JsonNode.class);
        Response restClientResponse = new Response(null, new HttpHeaders(), OK);
        when(restClient.call(any(URI.class), any(HttpMethod.class), any(HttpHeaders.class), any(JsonNode.class))).thenReturn(restClientResponse);

        sut.invoke(serviceId, "/redfish/v1/Systems/1", PATCH, new HttpHeaders(), requestBody);

        verify(restClient).call(
            any(URI.class),
            any(HttpMethod.class),
            any(HttpHeaders.class),
            eq(requestBody)
        );
    }

    @Test
    public void knowsIdOfServices() {
        Set<ServiceId> expectedIds = newHashSet(new ServiceId("X"), new ServiceId("Y"));
        when(finder.getServices()).thenReturn(expectedIds);

        assertThat(sut.getServices()).isEqualTo(finder.getServices());
    }

    private ArgumentMatcher<HttpHeaders> containsHeader(String headerName, String expectedHeaderValue) {
        return new ArgumentMatcher<HttpHeaders>() {
            @Override
            public boolean matches(Object arg) {
                if (arg instanceof HttpHeaders) {
                    List<String> actualHeaderValue = ((HttpHeaders) arg).get(headerName);

                    return Objects.equals(actualHeaderValue, singletonList(expectedHeaderValue));
                }

                return false;
            }
        };
    }
}
