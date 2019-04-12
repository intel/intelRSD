/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.security.filters;

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import com.intel.rsd.podm.security.AccessVerificationException;
import com.intel.rsd.podm.security.AccessVerifier;
import org.mockito.Mock;
import org.mockito.invocation.InvocationOnMock;
import org.springframework.cloud.client.ServiceInstance;
import org.springframework.cloud.client.loadbalancer.LoadBalancerClient;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpMethod;
import org.springframework.http.ResponseEntity;
import org.springframework.web.client.RestTemplate;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.net.URI;

import static org.mockito.Matchers.any;
import static org.mockito.Matchers.anyString;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.HttpStatus.UNAUTHORIZED;
import static org.springframework.http.ResponseEntity.status;
import static org.testng.Assert.assertEquals;

public class AccessVerifierTest {

    @Mock
    private LoadBalancerClient loadBalancerClientMock;

    @Mock
    private RestTemplate restTemplateMock;

    @Mock
    private VerifyAccessRequest verifyAccessRequestMock;

    @Mock
    private ServiceInstance aaaServiceInstanceMock;

    private AccessVerifier sut;

    @BeforeMethod
    public void beforeMethod() {
        initMocks(this);
        sut = new AccessVerifier(restTemplateMock, loadBalancerClientMock, "aaa");
        when(loadBalancerClientMock.choose(anyString())).thenReturn(aaaServiceInstanceMock);
        when(aaaServiceInstanceMock.getUri()).thenReturn(URI.create("http://localhost:8124/"));
    }

    @Test
    public void whenUnauthorizedThenPropagateStatus() {
        when(restTemplateMock.exchange(any(URI.class), any(HttpMethod.class), any(HttpEntity.class), any(Class.class))).thenAnswer(this::unauthorized);
        ResponseEntity response = sut.verifyAccess(verifyAccessRequestMock);

        assertEquals(response.getStatusCode(), UNAUTHORIZED);
    }

    @Test
    public void whenServerErrorThenPropagateStatus() {
        when(restTemplateMock.exchange(any(URI.class), any(HttpMethod.class), any(HttpEntity.class), any(Class.class))).thenAnswer(this::serverError);
        ResponseEntity response = sut.verifyAccess(verifyAccessRequestMock);
        assertEquals(response.getStatusCode(), INTERNAL_SERVER_ERROR);
    }

    @Test
    public void whenAuthenticatedThenStatusOk() {
        when(restTemplateMock.exchange(any(URI.class), any(HttpMethod.class), any(HttpEntity.class), any(Class.class))).thenAnswer(this::authorized);
        ResponseEntity response = sut.verifyAccess(verifyAccessRequestMock);
        assertEquals(response.getStatusCode(), OK);
    }

    @Test(expectedExceptions = AccessVerificationException.class)
    public void whenExceptionFromVerifierThenServiceUnavailable() {
        when(restTemplateMock.exchange(any(URI.class), any(HttpMethod.class), any(HttpEntity.class), any(Class.class)))
            .thenThrow(new AccessVerificationException("Error during rest call"));
        sut.verifyAccess(verifyAccessRequestMock);
    }

    private Object unauthorized(@SuppressWarnings("unused") InvocationOnMock invocation) {
        return status(UNAUTHORIZED).build();
    }

    private Object authorized(@SuppressWarnings("unused") InvocationOnMock invocation) {
        return status(OK).build();
    }

    private Object serverError(@SuppressWarnings("unused") InvocationOnMock invocation) {
        return status(INTERNAL_SERVER_ERROR).build();
    }
}
