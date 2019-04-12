/*
 * Copyright (c) 2018-2019 Intel Corporation
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
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.github.tomakehurst.wiremock.WireMockServer;
import com.intel.rsd.http.RsdClient;
import com.intel.rsd.service.detector.endpoint.verification.VerifyServicesTask;
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
import org.springframework.web.client.RestTemplate;
import org.testng.annotations.AfterClass;
import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import static com.github.tomakehurst.wiremock.client.WireMock.aResponse;
import static com.github.tomakehurst.wiremock.client.WireMock.get;
import static com.github.tomakehurst.wiremock.client.WireMock.urlEqualTo;
import static com.github.tomakehurst.wiremock.core.WireMockConfiguration.options;
import static java.util.UUID.randomUUID;
import static org.apache.http.entity.ContentType.APPLICATION_JSON;
import static org.assertj.core.api.Java6Assertions.assertThat;
import static org.springframework.boot.test.context.SpringBootTest.WebEnvironment.RANDOM_PORT;
import static org.springframework.http.HttpHeaders.CONTENT_TYPE;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.web.util.UriComponentsBuilder.fromUriString;

@AutoConfigureTestDatabase
@SpringBootTest(webEnvironment = RANDOM_PORT)
@TestPropertySource("classpath:detector.properties")
@TestExecutionListeners(MockitoTestExecutionListener.class)
@ActiveProfiles(value = {"truststore-based-verification", "https-only-service-registrar"}, inheritProfiles = false)
@EnableDiscoveryClient(autoRegister = false)
@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:ExecutableStatementCount"})
public class ServiceVerificationTest extends AbstractTestNGSpringContextTests {

    private static final String MANAGERS_COLLECTION_PATH = "/redfish/v1/Managers";

    private static final String SERVICE_UUID = randomUUID().toString();

    private static final String EXPECTED_MANAGER_PATH = fromUriString(MANAGERS_COLLECTION_PATH).pathSegment(SERVICE_UUID).build().toUri().toString();

    private static final String SERVICE_ROOT_OF_REDFISH_SERVICE = JsonNodeFactory.instance.objectNode()
        .put("UUID", SERVICE_UUID)
        .put("Name", "RedfishService")
        .put("@odata.id", EXPECTED_MANAGER_PATH)
        .toString();

    private WireMockServer serviceMock;

    /**
     * This rest template represents template which is used by PodM services to communicate with underlying Redfish services
     */
    @Autowired
    @RsdClient
    private RestTemplate podmApplicationRestTemplate;

    /**
     * This rest template represents client of PodM
     */
    @Autowired
    private TestRestTemplate podmClient;

    @Autowired
    private VerifyServicesTask verifyServicesTask;

    @BeforeClass
    public void prepareStubs() {
        prepareTrustedServiceMock();
        serviceMock.start();
    }

    @Test
    public void newlyRegisteredService_shouldBeRepresentedAsUntrustedManager() {
        val location = podmClient.postForLocation(
            MANAGERS_COLLECTION_PATH,
            JsonNodeFactory.instance.objectNode().put("RemoteRedfishServiceUri", "https://localhost:" + serviceMock.httpsPort() + "/redfish/v1")
        );

        assertThat(location.toString()).isEqualTo(EXPECTED_MANAGER_PATH);

        val responseEntity = podmClient.getForEntity(location, JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
        assertThat(responseEntity.getBody().with("Oem").with("Intel_RackScale").get("Trusted").asText()).isEqualToIgnoringCase("False");
    }

    @Test(dependsOnMethods = "newlyRegisteredService_shouldBeRepresentedAsUntrustedManager")
    public void afterUntrustedServiceVerification_representingManagerShouldBeTrusted() {
        verifyServicesTask.run();

        val responseEntity = podmClient.getForEntity(EXPECTED_MANAGER_PATH, JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
        assertThat(responseEntity.getBody().with("Oem").with("Intel_RackScale").get("Trusted").asText()).isEqualToIgnoringCase("True");
    }

    @Test(dependsOnMethods = "afterUntrustedServiceVerification_representingManagerShouldBeTrusted")
    public void whenApplicationTryToTalkWithTrustedService_200ShouldBeReturned() {
        val responseEntity = podmApplicationRestTemplate.getForEntity("https://localhost:" + serviceMock.httpsPort() + "/redfish/v1", JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
        assertThat(responseEntity.getBody().get("@odata.id").asText()).isEqualTo(EXPECTED_MANAGER_PATH);
    }

    @Test(dependsOnMethods = "whenApplicationTryToTalkWithTrustedService_200ShouldBeReturned")
    public void whenServiceCertificateIsNotTrustedAnymore_representingManagerShouldBeUntrusted() {
        restartServerMockWithUntrustedCertificate();
        verifyServicesTask.run();

        val responseEntity = podmClient.getForEntity(EXPECTED_MANAGER_PATH, JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
        assertThat(responseEntity.getBody().with("Oem").with("Intel_RackScale").get("Trusted").asText()).isEqualToIgnoringCase("False");
    }

    @AfterClass
    public void tearDown() {
        serviceMock.stop();
    }

    private void prepareTrustedServiceMock() {
        serviceMock = new WireMockServer(
            options()
                .keystorePath("classpath:/truststore.jks")
                .keystorePassword("rsa123")
                .dynamicHttpsPort()
        );
        stubGetOnRoot();
    }

    private void restartServerMockWithUntrustedCertificate() {
        val httpsPort = serviceMock.httpsPort();
        serviceMock.stop();
        serviceMock = new WireMockServer(
            options()
                .httpsPort(httpsPort));
        stubGetOnRoot();
        serviceMock.start();
    }

    private void stubGetOnRoot() {
        serviceMock.stubFor(get(urlEqualTo("/redfish/v1"))
            .willReturn(
                aResponse()
                    .withStatus(OK.value())
                    .withHeader(CONTENT_TYPE, APPLICATION_JSON.getMimeType())
                    .withBody(SERVICE_ROOT_OF_REDFISH_SERVICE)
            )
        );
    }

}
