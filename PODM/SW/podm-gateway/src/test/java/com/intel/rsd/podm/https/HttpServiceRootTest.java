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

package com.intel.rsd.podm.https;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.podm.PodmGatewayApplication;
import com.intel.rsd.podm.security.AccessVerifier;
import lombok.SneakyThrows;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.boot.test.mock.mockito.MockitoTestExecutionListener;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.TestExecutionListeners;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import javax.servlet.FilterChain;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import java.net.URI;

import static org.mockito.Mockito.any;
import static org.mockito.Mockito.doNothing;
import static org.testng.Assert.assertEquals;

@SpringBootTest(classes = PodmGatewayApplication.class, webEnvironment = SpringBootTest.WebEnvironment.DEFINED_PORT,
    properties = {"zuul.routes.resource-manager.path=/redfish/v1/**", "server.port=8010", "server.httpsPort=8020"})
@EnableDiscoveryClient(autoRegister = false)
@TestExecutionListeners(MockitoTestExecutionListener.class)
public class HttpServiceRootTest extends AbstractTestNGSpringContextTests {

    @MockBean
    private AccessVerifier accessVerifier;

    @MockBean
    private ServiceRootOnlyFilter serviceRootOnlyFilter;

    @Autowired
    private TestRestTemplate testRestTemplate;

    @Test
    @SneakyThrows
    public void whenTryingToAccessServiceRootResourceWithConfiguredHttpsThenShouldReturnItViaHttpAndHttps() {
        doNothing().when(serviceRootOnlyFilter).doFilter(any(ServletRequest.class), any(ServletResponse.class), any(FilterChain.class));
        ResponseEntity<JsonNode> responseEntity = testRestTemplate.getForEntity(URI.create("http://localhost:8010/redfish/v1"), JsonNode.class);
        assertEquals(responseEntity.getStatusCode(), HttpStatus.OK);
        ResponseEntity<JsonNode> httpsResponseEntity = testRestTemplate.getForEntity(URI.create("https://localhost:8020/redfish/v1"), JsonNode.class);
        assertEquals(httpsResponseEntity.getStatusCode(), HttpStatus.OK);
    }
}
