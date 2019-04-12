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

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import com.intel.rsd.podm.PodmGatewayApplication;
import com.intel.rsd.podm.security.AccessVerificationException;
import com.intel.rsd.podm.security.AccessVerifier;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.boot.test.mock.mockito.MockitoTestExecutionListener;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.context.annotation.Import;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.TestExecutionListeners;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;

import static org.mockito.Matchers.any;
import static org.mockito.Mockito.reset;
import static org.mockito.Mockito.when;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;
import static org.testng.Assert.assertEquals;

@SpringBootTest(classes = PodmGatewayApplication.class, webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT,
    properties = {"zuul.routes.node-composer.path=/redfish/v1/Nodes/**"})
@EnableDiscoveryClient(autoRegister = false)
@TestExecutionListeners(MockitoTestExecutionListener.class)
@Import(TestOnlyZuulConfig.class)
public class AccessVerifierFilterTest extends AbstractTestNGSpringContextTests {

    @MockBean
    private AccessVerifier accessVerifier;

    @Autowired
    private TestRestTemplate testRestTemplate;

    @DataProvider(name = "restOperations")
    public static Object[] restOperations() {
        return new Object[]{HttpStatus.UNAUTHORIZED, HttpStatus.OK, HttpStatus.SERVICE_UNAVAILABLE, HttpStatus.INTERNAL_SERVER_ERROR};
    }

    @BeforeMethod
    public void beforeMethod() {
        reset(accessVerifier);
    }

    @Test(dataProvider = "restOperations")
    public void whenVerifierRespondsThenPropagateResponseStatus(HttpStatus verifierResponseStatus) {
        when(accessVerifier.verifyAccess(any(VerifyAccessRequest.class))).thenReturn(ResponseEntity.status(verifierResponseStatus).build());
        ResponseEntity<ObjectNode> responseEntity = testRestTemplate.getForEntity(URI.create("/redfish/v1/Nodes"), ObjectNode.class);
        assertEquals(responseEntity.getStatusCode(), verifierResponseStatus);
    }

    @Test
    public void whenExceptionOnVerifierThenStatusServiceUnavailable() {
        when(accessVerifier.verifyAccess(any(VerifyAccessRequest.class))).thenThrow(new AccessVerificationException("Something went wrong."));
        ResponseEntity<ObjectNode> responseEntity = testRestTemplate.getForEntity(URI.create("/redfish/v1/Nodes"), ObjectNode.class);
        assertEquals(responseEntity.getStatusCode(), SERVICE_UNAVAILABLE);
    }
}
