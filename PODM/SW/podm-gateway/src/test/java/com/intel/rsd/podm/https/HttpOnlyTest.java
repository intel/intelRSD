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
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.boot.test.mock.mockito.MockitoTestExecutionListener;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.test.context.TestExecutionListeners;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.springframework.web.client.ResourceAccessException;
import org.testng.annotations.Test;

import javax.servlet.FilterChain;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import java.io.IOException;
import java.net.URI;

import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doNothing;

@SpringBootTest(classes = PodmGatewayApplication.class, webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT,
    properties = {"zuul.routes.resource-manager.path=/redfish/v1/**", "server.ssl.enabled=false"})
@EnableDiscoveryClient(autoRegister = false)
@TestExecutionListeners(MockitoTestExecutionListener.class)
public class HttpOnlyTest extends AbstractTestNGSpringContextTests {

    @MockBean
    private AccessVerifier accessVerifier;

    @MockBean
    private ServiceRootOnlyFilter serviceRootOnlyFilter;

    @Autowired
    private TestRestTemplate testRestTemplate;

    @Test(expectedExceptions = ResourceAccessException.class)
    public void whenSslTlsIsNotConfiguredThenShouldNotExposeHttpsPort() throws IOException, ServletException {
        doNothing().when(serviceRootOnlyFilter).doFilter(any(ServletRequest.class), any(ServletResponse.class), any(FilterChain.class));
        testRestTemplate.getForEntity(URI.create("https://localhost:8443/redfish/v1"), ObjectNode.class);
    }
}
