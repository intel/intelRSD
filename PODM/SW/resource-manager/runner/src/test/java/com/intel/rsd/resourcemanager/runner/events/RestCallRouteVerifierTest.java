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

package com.intel.rsd.resourcemanager.runner.events;

import org.mockito.Mock;
import org.springframework.http.ResponseEntity;
import org.springframework.web.client.ResourceAccessException;
import org.springframework.web.client.RestOperations;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.net.URI;

import static org.junit.Assert.assertFalse;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpStatus.INTERNAL_SERVER_ERROR;

public class RestCallRouteVerifierTest {

    @Mock
    private RestOperations restOperationsMock;

    private RestCallRouteVerifier sut;

    private final URI dummyRedfishRootUri = URI.create("http://10.4.0.1:1312/redfish/v1");

    @BeforeMethod
    public void setUp() {
        initMocks(this);
        sut = new RestCallRouteVerifier(restOperationsMock);
    }

    @Test
    public void whenErrorStatus_thenIndicateNoRoute() {
        when(restOperationsMock.getForEntity(any(), any())).thenReturn(new ResponseEntity(null, INTERNAL_SERVER_ERROR));
        boolean reachable = sut.isReachable(dummyRedfishRootUri);

        assertFalse(reachable);
    }

    @Test
    public void whenException_thenIndicateNoRoute() {
        when(restOperationsMock.getForEntity(any(), any())).thenThrow(new ResourceAccessException(""));
        boolean reachable = sut.isReachable(dummyRedfishRootUri);

        assertFalse(reachable);
    }
}
