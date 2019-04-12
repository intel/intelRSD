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

import com.fasterxml.jackson.databind.node.ObjectNode;
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

import java.net.URI;

import static org.testng.Assert.assertEquals;

@SpringBootTest(classes = PodmGatewayApplication.class, webEnvironment = SpringBootTest.WebEnvironment.DEFINED_PORT,
    properties = {"zuul.routes.resource-manager.path=/redfish/v1/**", "server.port=8011", "server.httpsPort=8022"})
@EnableDiscoveryClient(autoRegister = false)
@TestExecutionListeners(MockitoTestExecutionListener.class)
public class HttpToHttpsRedirectTest extends AbstractTestNGSpringContextTests {

    @MockBean
    private AccessVerifier accessVerifier;

    @Autowired
    private TestRestTemplate testRestTemplate;

    @Test
    @SneakyThrows
    public void whenTryingToAccessNonServiceRootResourceThenShouldRedirectToHttps() {
        ResponseEntity<ObjectNode> responseEntity = testRestTemplate.getForEntity(URI.create("http://localhost:8011/redfish/v1/Chassis"), ObjectNode.class);
        assertEquals(responseEntity.getHeaders().getLocation().toString(), "https://localhost:8022/redfish/v1/Chassis");
        assertEquals(responseEntity.getStatusCode(), HttpStatus.FOUND);
    }
}
