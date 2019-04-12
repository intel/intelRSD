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

package com.intel.rsd.service.detector.endpoint;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.mock.mockito.MockitoTestExecutionListener;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.TestExecutionListeners;
import org.springframework.test.context.TestPropertySource;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import static org.assertj.core.api.Java6Assertions.assertThat;
import static org.springframework.boot.test.context.SpringBootTest.WebEnvironment.RANDOM_PORT;
import static org.springframework.http.HttpStatus.OK;

@AutoConfigureTestDatabase
@EnableDiscoveryClient(autoRegister = false)
@SpringBootTest(webEnvironment = RANDOM_PORT)
@TestPropertySource("classpath:detector.properties")
@TestExecutionListeners(MockitoTestExecutionListener.class)
@ActiveProfiles({"truststore-based-verification", "https-only-service-registrar"})
public class RsdInitializationDefaultConfig extends AbstractTestNGSpringContextTests {

    @Autowired
    private TestRestTemplate podmClient;

    @Test
    public void managersCollectionIsAchievable() {
        val responseEntity = podmClient.getForEntity("/redfish/v1/Managers", JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
    }
}
