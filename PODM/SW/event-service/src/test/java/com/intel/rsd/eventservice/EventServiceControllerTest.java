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

package com.intel.rsd.eventservice;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.dto.events.SubscriptionRequest;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.boot.test.web.client.TestRestTemplate;
import org.springframework.cloud.client.discovery.EnableDiscoveryClient;
import org.springframework.http.HttpEntity;
import org.springframework.http.ResponseEntity;
import org.springframework.test.context.testng.AbstractTestNGSpringContextTests;
import org.testng.annotations.Test;

import java.net.URI;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.rsd.dto.ODataId.odataId;
import static org.assertj.core.api.Java6Assertions.assertThat;
import static org.springframework.http.HttpMethod.DELETE;
import static org.springframework.http.HttpMethod.PATCH;
import static org.springframework.http.HttpStatus.BAD_REQUEST;
import static org.springframework.http.HttpStatus.CREATED;
import static org.springframework.http.HttpStatus.METHOD_NOT_ALLOWED;
import static org.springframework.http.HttpStatus.NOT_FOUND;
import static org.springframework.http.HttpStatus.OK;

@EnableDiscoveryClient(autoRegister = false)
@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
public class EventServiceControllerTest extends AbstractTestNGSpringContextTests {
    @Autowired
    private EventServiceConfiguration eventServiceConfiguration;

    @Autowired
    private TestRestTemplate restTemplate;

    @Test
    public void getOnEventServiceResource() throws Exception {
        ResponseEntity<JsonNode> eventServiceResource = restTemplate.getForEntity("/redfish/v1/EventService", JsonNode.class);

        assertThat(eventServiceResource.getStatusCode()).isEqualTo(OK);
        assertThat(eventServiceResource.getBody().get("DeliveryRetryAttempts").asInt())
            .isEqualTo(eventServiceConfiguration.getDeliveryRetryAttempts());
        assertThat(eventServiceResource.getBody().get("DeliveryRetryIntervalSeconds").asInt())
            .isEqualTo(eventServiceConfiguration.getDeliveryRetryIntervalSeconds());
    }

    @Test
    public void getOnSubscriptionsResource() throws Exception {
        ResponseEntity<JsonNode> eventServiceResource = restTemplate.getForEntity("/redfish/v1/EventService/Subscriptions", JsonNode.class);

        assertThat(eventServiceResource.getStatusCode()).isEqualTo(OK);
        assertThat(eventServiceResource.getBody().get("@odata.type").asText()).isEqualTo("#EventDestinationCollection.EventDestinationCollection");
    }

    @Test
    public void emptySubscriptionRegistrationRequest_shouldReturnBadRequest() throws Exception {
        val eventServiceResource = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            JsonNodeFactory.instance.objectNode(),
            JsonNode.class
        );

        assertThat(eventServiceResource.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithInvalidName_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        request.setName("");

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            request,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithNullName_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        request.setName(null);

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            request,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithInvalidContext_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        request.setContext("");

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            request,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithInvalidDestination_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        request.setDestination(URI.create(""));

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            request,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithInvalidEventTypes_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        val mapper = new ObjectMapper();
        ObjectNode requestAsObjectNode = mapper.valueToTree(request);
        val invalidEventTypes = JsonNodeFactory.instance.arrayNode().add("a").add("b");
        requestAsObjectNode.set("EventTypes", invalidEventTypes);

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            requestAsObjectNode,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithInvalidOriginResourcesContainingOdataIdWithIllegalChar_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        val mapper = new ObjectMapper();
        ObjectNode requestAsObjectNode = mapper.valueToTree(request);
        val originResources = JsonNodeFactory.instance.arrayNode().add(JsonNodeFactory.instance.objectNode().put("@odata.id", ">"));
        requestAsObjectNode.set("OriginResources", originResources);

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            requestAsObjectNode,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithNullOriginResources_shouldReturnAppropriateLocation() throws Exception {
        val request = createValidSubscriptionRequest();
        val mapper = new ObjectMapper();
        ObjectNode requestAsObjectNode = mapper.valueToTree(request);
        requestAsObjectNode.set("OriginResources", JsonNodeFactory.instance.nullNode());

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            requestAsObjectNode,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(CREATED);
        assertThat(subscriptionRegistrationResponse.getHeaders().getLocation().toString()).contains("/redfish/v1/EventService/Subscriptions/");
    }

    @Test
    public void subscriptionRegistrationWithInvalidOriginResourcesContainingNull_shouldReturnAppropriateLocation() throws Exception {
        val request = createValidSubscriptionRequest();
        val mapper = new ObjectMapper();
        ObjectNode requestAsObjectNode = mapper.valueToTree(request);
        val originResources = JsonNodeFactory.instance.arrayNode().add(JsonNodeFactory.instance.nullNode());
        requestAsObjectNode.set("OriginResources", originResources);

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            requestAsObjectNode,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void subscriptionRegistrationWithInvalidOriginResourcesContainingNullOdataId_shouldReturnBadRequest() throws Exception {
        val request = createValidSubscriptionRequest();
        val mapper = new ObjectMapper();
        ObjectNode requestAsObjectNode = mapper.valueToTree(request);
        val originResources = JsonNodeFactory.instance.arrayNode().add(JsonNodeFactory.instance.objectNode().put("@odata.id", (String) null));
        requestAsObjectNode.set("OriginResources", originResources);

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            requestAsObjectNode,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void validSubscriptionRegistrationRequest_shouldReturnAppropriateLocation() throws Exception {
        val request = createValidSubscriptionRequest();

        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            request,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(CREATED);
        assertThat(subscriptionRegistrationResponse.getHeaders().getLocation().toString()).contains("/redfish/v1/EventService/Subscriptions/");
    }

    @Test
    public void validSubscriptionRegistrationRequestWithDefinedOriginResources_shouldReturnAppropriateLocation() throws Exception {
        val request = createValidSubscriptionRequest();
        request.setOriginResources(newHashSet(odataId("/abc/def")));
        val subscriptionRegistrationResponse = restTemplate.postForEntity(
            "/redfish/v1/EventService/Subscriptions",
            request,
            JsonNode.class
        );

        assertThat(subscriptionRegistrationResponse.getStatusCode()).isEqualTo(CREATED);
        assertThat(subscriptionRegistrationResponse.getHeaders().getLocation().toString()).contains("/redfish/v1/EventService/Subscriptions/");
    }

    private SubscriptionRequest createValidSubscriptionRequest() {
        return SubscriptionRequestFactory.create("name", URI.create("http://localhost"), "context", null, newHashSet(EventType.values()));
    }

    @Test
    public void subscriptionRegistrationAndGetOnSubscriptionsCollection_shouldContainLinkToSubscription() throws Exception {
        val subscriptionsCollectionResponseBeforeRegistration = restTemplate.getForEntity("/redfish/v1/EventService/Subscriptions", JsonNode.class);
        int membersCountBeforeRegistration = subscriptionsCollectionResponseBeforeRegistration.getBody().get("Members@odata.count").asInt();
        val request = createValidSubscriptionRequest();
        val subscriptionUri = restTemplate.postForLocation("/redfish/v1/EventService/Subscriptions", request);

        val subscriptionsCollectionResponse = restTemplate.getForEntity("/redfish/v1/EventService/Subscriptions", JsonNode.class);
        val subscriptionsCollectionResponseBody = subscriptionsCollectionResponse.getBody();

        assertThat(subscriptionsCollectionResponse.getStatusCode()).isEqualTo(OK);
        assertThat(subscriptionsCollectionResponseBody.get("Members@odata.count").asInt()).isEqualTo(membersCountBeforeRegistration + 1);
        assertThat(subscriptionsCollectionResponseBody.get("Members").isArray()).isTrue();
        assertThat(subscriptionsCollectionResponseBody.get("Members"))
            .contains(JsonNodeFactory.instance.objectNode().put("@odata.id", subscriptionUri.toString()));
    }

    @Test
    public void subscriptionRegistrationAndGet_shouldReturnAppropriateSubscriptionResource() throws Exception {
        val request = createValidSubscriptionRequest();

        val subscriptionUri = restTemplate.postForLocation("/redfish/v1/EventService/Subscriptions", request);
        val subscriptionResourceResponse = restTemplate.getForEntity(subscriptionUri, JsonNode.class);
        val subscriptionBody = subscriptionResourceResponse.getBody();
        TextNode[] expectedEventTypes = request.getEventTypes().stream()
            .map(eventType -> new TextNode(eventType.toString()))
            .toArray(TextNode[]::new);

        assertThat(subscriptionResourceResponse.getStatusCode()).isEqualTo(OK);
        assertThat(subscriptionBody.get("Context").asText("")).isEqualTo(request.getContext());
        assertThat(subscriptionBody.get("Destination").asText("")).isEqualTo(request.getDestination().toString());
        assertThat(subscriptionBody.get("Name").asText("")).isEqualTo(request.getName());
        assertThat(subscriptionBody.get("EventTypes").isArray()).isTrue();
        assertThat(subscriptionBody.get("EventTypes")).containsExactlyInAnyOrder(expectedEventTypes);
    }

    @Test
    public void getOnNonExistingSubscription_shouldReturnNotFound() throws Exception {
        val response = restTemplate.getForEntity("/redfish/v1/EventService/Subscriptions/666", JsonNode.class);

        assertThat(response.getStatusCode()).isEqualTo(NOT_FOUND);
    }

    @Test
    public void getOnNSubscriptionWithIncorrectId_shouldReturnBadRequest() throws Exception {
        val response = restTemplate.getForEntity("/redfish/v1/EventService/Subscriptions/nonExistingSubscription", JsonNode.class);

        assertThat(response.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void removeExistingSubscriptionAndGet_shouldReturnNotFound() throws Exception {
        val request = createValidSubscriptionRequest();
        val subscriptionUri = restTemplate.postForLocation("/redfish/v1/EventService/Subscriptions", request);

        restTemplate.delete(subscriptionUri);
        val response = restTemplate.getForEntity(subscriptionUri, JsonNode.class);

        assertThat(response.getStatusCode()).isEqualTo(NOT_FOUND);
    }

    @Test
    public void removeNonExistingSubscription_shouldReturnNotFound() throws Exception {
        val response = restTemplate.exchange("/redfish/v1/EventService/Subscriptions/666", DELETE, HttpEntity.EMPTY, JsonNode.class);

        assertThat(response.getStatusCode()).isEqualTo(NOT_FOUND);
    }

    @Test
    public void removeOnNSubscriptionWithIncorrectId_shouldReturnBadRequest() throws Exception {
        val response = restTemplate.exchange("/redfish/v1/EventService/Subscriptions/nonExistingSubscription", DELETE, HttpEntity.EMPTY, JsonNode.class);

        assertThat(response.getStatusCode()).isEqualTo(BAD_REQUEST);
    }

    @Test
    public void patchOnExistingSubscription_shouldReturnMethodNotAllowed() throws Exception {
        val request = createValidSubscriptionRequest();
        val subscriptionUri = restTemplate.postForLocation("/redfish/v1/EventService/Subscriptions", request);

        val response = restTemplate.exchange(subscriptionUri, PATCH, HttpEntity.EMPTY, JsonNode.class);

        assertThat(response.getStatusCode()).isEqualTo(METHOD_NOT_ALLOWED);
    }
}
