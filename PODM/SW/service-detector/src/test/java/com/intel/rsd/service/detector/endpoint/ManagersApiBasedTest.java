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
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.intel.rsd.service.detector.data.RemoteServiceType;
import com.intel.rsd.service.detector.ServiceDescriptor;
import com.intel.rsd.service.detector.ServiceEndpoint;
import com.intel.rsd.service.detector.endpoint.registration.ServiceDescribingException;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.autoconfigure.jdbc.AutoConfigureTestDatabase;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.mock.mockito.MockBean;
import org.springframework.boot.test.mock.mockito.MockitoTestExecutionListener;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.http.RequestEntity;
import org.springframework.test.context.ActiveProfiles;
import org.springframework.test.context.TestExecutionListeners;
import org.springframework.test.context.TestPropertySource;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;
import java.util.UUID;

import static com.intel.rsd.service.detector.data.RemoteServiceType.DiscoveryService;
import static com.intel.rsd.service.detector.data.RemoteServiceType.ResourceService;
import static java.net.URI.create;
import static java.util.stream.StreamSupport.stream;
import static org.assertj.core.api.Java6Assertions.assertThat;
import static org.mockito.Mockito.when;
import static org.springframework.boot.test.context.SpringBootTest.WebEnvironment.RANDOM_PORT;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CONFLICT;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.NO_CONTENT;
import static org.springframework.http.HttpStatus.OK;
import static org.springframework.http.HttpStatus.SERVICE_UNAVAILABLE;

@AutoConfigureTestDatabase
@EnableDiscoveryClient(autoRegister = false)
@SpringBootTest(webEnvironment = RANDOM_PORT)
@TestPropertySource("classpath:detector.properties")
@TestExecutionListeners(MockitoTestExecutionListener.class)
@ActiveProfiles({"no-verification", "https-only-service-registrar"})
@SuppressWarnings({"checkstyle:MagicNumber", "checkstyle:ExecutableStatementCount"})
@Test(groups = "ordered")
public class ManagersApiBasedTest extends AbstractTestNGSpringContextTests {

    private static final String EXPANDED_MANAGERS_COLLECTION_URI = "/redfish/v1/Managers?$expand=.($levels=1)";

    private static final String MANAGERS_COLLECTION_URI = "/redfish/v1/Managers";

    @Autowired
    private TestRestTemplate restTemplate;

    @MockBean
    private ServiceDescriptor serviceDescriptor;

    @Test(groups = "preflight")
    public void managersCollectionIsReturningAppropriateResponse() {
        val responseEntity = restTemplate.getForEntity(MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
        assertThat(responseEntity.getBody().at("/@odata.type").asText()).isEqualTo("#ManagerCollection.ManagerCollection");
        assertThat(responseEntity.getBody().withArray("Members").size()).isEqualTo(0);
    }

    @Test(groups = "preflight")
    public void expandedManagersCollectionIsReturningAppropriateResponse() {
        val responseEntity = restTemplate.getForEntity(EXPANDED_MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(OK);
        assertThat(responseEntity.getBody().at("/@odata.type").asText()).isEqualTo("#ManagerCollection.ManagerCollection");
        assertThat(responseEntity.getBody().withArray("Members").size()).isEqualTo(0);
    }

    @Test(groups = "preflight")
    public void whenNotSupportedExpandQueryIsReceivedThen400IsExpected() {
        val notSupportedUri = "/redfish/v1/Managers?$expand=.($levels=2)";
        val responseEntity = restTemplate.getForEntity(notSupportedUri, JsonNode.class);
        assertThat(responseEntity.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test(groups = "registrations", dependsOnGroups = "preflight")
    public void whenRegistrationOfHttpServiceWasRequestedThen400IsExpected() {
        val responseEntity = restTemplate.postForEntity(
            MANAGERS_COLLECTION_URI,
            JsonNodeFactory.instance.objectNode().put("RemoteRedfishServiceUri", "http://anyHttpUri"),
            JsonNode.class
        );
        assertThat(responseEntity.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test(groups = "registrations", dependsOnGroups = "preflight")
    public void whenBadRequestIsProvidedThen400IsExpected() {
        val responseEntity = restTemplate.postForEntity(
            MANAGERS_COLLECTION_URI,
            JsonNodeFactory.instance.objectNode(),
            JsonNode.class
        );
        assertThat(responseEntity.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test(groups = "registrations", dependsOnGroups = "preflight")
    public void whenProvidedServiceCannotBeCorrectlyDescribedThen503IsExpected() {
        val serviceUri = create("https://localhost:1234/redfish/v1");

        when(serviceDescriptor.describe(serviceUri)).thenThrow(new ServiceDescribingException("Cannot describe"));

        val responseEntity = restTemplate.postForEntity(
            MANAGERS_COLLECTION_URI,
            JsonNodeFactory.instance.objectNode().put("RemoteRedfishServiceUri", serviceUri.toString()),
            JsonNode.class
        );

        assertThat(responseEntity.getStatusCode()).isEqualTo(SERVICE_UNAVAILABLE);
    }

    @Test(groups = "registrations", dependsOnGroups = "preflight", dataProvider = "registrations")
    public void whenNewManagerIsRegisteredItShouldBeExposedOnManagersCollection(int invocationNumber, RemoteServiceType remoteServiceType,
                                                                                String serviceUri, UUID serviceUuid) {

        when(serviceDescriptor.describe(create(serviceUri))).thenReturn(new ServiceEndpoint(serviceUuid, create(serviceUri), remoteServiceType));

        val responseEntity = restTemplate.postForEntity(
            MANAGERS_COLLECTION_URI,
            JsonNodeFactory.instance.objectNode().put("RemoteRedfishServiceUri", serviceUri),
            JsonNode.class
        );

        assertThat(responseEntity.getStatusCode()).isEqualTo(CREATED);

        val detectionServiceResponse = restTemplate.getForEntity(MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(detectionServiceResponse.getStatusCode()).isEqualTo(OK);

        val members = (ArrayNode) detectionServiceResponse.getBody().withArray("Members");
        assertThat(members.size()).isEqualTo(invocationNumber);
        assertThat(getNthElement(invocationNumber, members).at("/@odata.id").asText()).contains(serviceUuid.toString());

        val expandedManagersResponseEntity = restTemplate.getForEntity(EXPANDED_MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(expandedManagersResponseEntity.getStatusCode()).isEqualTo(OK);

        val expandedManagersMembers = (ArrayNode) expandedManagersResponseEntity.getBody().withArray("Members");
        assertThat(expandedManagersMembers.size()).isEqualTo(invocationNumber);

        val nthElement = getNthElement(invocationNumber, expandedManagersMembers);
        assertThat(nthElement.at("/@odata.id").asText()).contains(serviceUuid.toString());
        assertThat(nthElement.at("/RemoteRedfishServiceUri").asText()).isEqualTo(serviceUri);
        assertThat(nthElement.at("/ServiceEntryPointUUID").asText()).isEqualTo(serviceUuid.toString());
        assertThat(nthElement.at("/Id").asText()).isEqualTo(serviceUuid.toString());
    }

    @Test(groups = "secondregistrations", dependsOnGroups = "registrations", dataProvider = "registrations")
    public void whenAlreadyRegisteredManagerIsRegisteredAgainItShouldReturn409(int invocationNumber, RemoteServiceType remoteServiceType,
                                                                               String serviceUri, UUID serviceUuid) {

        when(serviceDescriptor.describe(create(serviceUri))).thenReturn(new ServiceEndpoint(serviceUuid, create(serviceUri), remoteServiceType));

        val responseEntity = restTemplate.postForEntity(
            MANAGERS_COLLECTION_URI,
            JsonNodeFactory.instance.objectNode().put("RemoteRedfishServiceUri", serviceUri),
            JsonNode.class
        );

        assertThat(responseEntity.getStatusCode()).isEqualTo(CONFLICT);
    }

    @Test(groups = "unregistrations", dependsOnGroups = "secondregistrations", dataProvider = "unregistrations")
    public void whenDeletionOfManagerIsRequestedThen204ShouldBeReturned(URI managerUri) {
        val getManagersCollectionResponse = restTemplate.getForEntity(MANAGERS_COLLECTION_URI, JsonNode.class);
        val numberOfRegisteredManagers = getManagersCollectionResponse.getBody().get("Members@odata.count").asInt();

        val deletionRequest = RequestEntity.delete(managerUri).build();
        val detectionServiceResponse = restTemplate.exchange(deletionRequest, JsonNode.class);
        assertThat(detectionServiceResponse.getStatusCode()).isEqualTo(NO_CONTENT);

        val getManagerResponse = restTemplate.getForEntity(managerUri, JsonNode.class);
        assertThat(getManagerResponse.getStatusCode()).isEqualTo(NOT_FOUND);

        val getManagersCollectionResponseAfterDeletion = restTemplate.getForEntity(MANAGERS_COLLECTION_URI, JsonNode.class);
        val numberOfRegisteredManagersAfterDeletion = getManagersCollectionResponseAfterDeletion.getBody().get("Members@odata.count").asInt();
        assertThat(numberOfRegisteredManagersAfterDeletion).isEqualTo(numberOfRegisteredManagers - 1);
    }

    @Test(dependsOnGroups = "unregistrations")
    public void flatManagerCollectionShouldHaveZeroMembers() {
        val getFlatManagerCollectionResponse = restTemplate.getForEntity(MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(getFlatManagerCollectionResponse.getStatusCode()).isEqualTo(OK);
        assertThat(getFlatManagerCollectionResponse.getBody().get("Members@odata.count").asInt()).isEqualTo(0);
        assertThat(getFlatManagerCollectionResponse.getBody().withArray("Members").size()).isEqualTo(0);
    }

    @Test(dependsOnGroups = "unregistrations")
    public void expandedManagerCollectionShouldHaveZeroMembers() {
        val getFlatManagerCollectionResponse = restTemplate.getForEntity(EXPANDED_MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(getFlatManagerCollectionResponse.getStatusCode()).isEqualTo(OK);
        assertThat(getFlatManagerCollectionResponse.getBody().get("Members@odata.count").asInt()).isEqualTo(0);
        assertThat(getFlatManagerCollectionResponse.getBody().withArray("Members").size()).isEqualTo(0);

    }

    private JsonNode getNthElement(int n, ArrayNode arrayNode) {
        return arrayNode.get(n - 1);
    }

    @DataProvider
    private Object[][] registrations() {
        return new Object[][]{
            new Object[]{1, ResourceService, "https://localhost:1/redfish/v1", UUID.fromString("ed4288ca-f3b2-11e8-8eb2-f2801f1b9fd1")},
            new Object[]{2, ResourceService, "https://localhost:2/redfish/v1", UUID.fromString("ed4288ca-f3b2-11e8-8eb2-f2801f1b9fd2")},
            new Object[]{3, DiscoveryService, "https://localhost:3/redfish/v1", UUID.fromString("ed4288ca-f3b2-11e8-8eb2-f2801f1b9fd3")}
        };
    }

    @DataProvider
    private Object[][] unregistrations() {
        val managersCollection = restTemplate.getForEntity(MANAGERS_COLLECTION_URI, JsonNode.class);
        assertThat(managersCollection.getStatusCode()).isEqualTo(OK);
        return stream(managersCollection.getBody().withArray("Members").spliterator(), false)
            .map(member -> new Object[]{URI.create(member.get("@odata.id").asText())})
            .toArray(Object[][]::new);
    }
}
