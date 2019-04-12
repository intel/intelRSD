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

package com.intel.rsd.resourcemanager.runner.events;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.json.ArrayNodeCollector;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration.Receiver.FixedProvider;
import com.intel.rsd.resourcemanager.runner.requiredlayer.RequiredLayer;
import com.intel.rsd.resourcemanager.runner.requiredlayer.RequiredServiceFinder;
import lombok.val;
import org.mockito.InjectMocks;
import org.mockito.Mock;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import java.net.URI;

import static com.intel.rsd.dto.events.EventType.Alert;
import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static java.net.URI.create;
import static java.util.Arrays.stream;
import static java.util.Collections.singleton;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.doReturn;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.spy;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.mockito.MockitoAnnotations.initMocks;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.http.HttpStatus.OK;

public class EventSubscriptionTaskTest {

    private ServiceId serviceId = new ServiceId("anyServiceId");

    @Mock
    private RequiredLayer requiredLayer;

    @Mock
    private RequiredServiceFinder requiredServiceFinder;

    @Mock
    private EventsConfiguration eventsConfiguration;

    @Mock
    private SubscriptionDestinationProvider subscriptionDestinationProvider;

    @InjectMocks
    private EventSubscriptionTask sut;

    @BeforeMethod
    public void setUp() {
        sut = spy(new EventSubscriptionTask(serviceId));
        initMocks(this);
        prepareEventsConfigurationStubbing();
        prepareSubscriptionDestinationProviderStubbing();
        prepareRequiredServiceFinderStubbing();
    }

    @Test
    public void whenSubscriptionIsNotRegistered_itShouldBeAdded() throws Exception {
        doReturn(createOkResponseWithBody(readJsonFromFile(this, "emptySubscriptionsCollection.json")))
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(GET), any(), any());

        doReturn(createOkResponse())
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(POST), any(), any());

        prepareFindSupportedEventTypesStubbing(Alert);
        sut.run();

        verify(requiredLayer).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    @Test
    public void whenEventServiceOnRemoteSystemDoesNotExposeInfoAboutSupportedEventTypes_subscriptionShouldNotBeRegistered() throws Exception {
        doReturn(createOkResponseWithBody(readJsonFromFile(this, "emptySubscriptionsCollection.json")))
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(GET), any(), any());

        prepareFindSupportedEventTypesStubbing();

        sut.run();
        verify(requiredLayer, never()).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    @Test
    public void whenAlreadyRegisteredSubscriptionIsNotPodMSubscription_podmSubscriptionShouldBeAdded() throws Exception {
        doReturn(createOkResponseWithBody(readJsonFromFile(this, "singleElementSubscriptionCollection.json")))
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(GET), any(), any());


        val existingSubscriptionBody = readJsonFromFile(this, "subscription1.json").put("Destination", "/nonPodMDestination");
        doReturn(createOkResponseWithBody(existingSubscriptionBody))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions/1"), eq(GET), any(), any());

        doReturn(createOkResponse())
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(POST), any(), any());

        prepareFindSupportedEventTypesStubbing(Alert);
        sut.run();

        verify(requiredLayer).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    @Test
    public void whenKnownServiceIsAlreadySubscribed_subscriptionShouldNotBeRegisteredAgain() throws Exception {

        val nonEmptySubscriptionsCollectionBody = readJsonFromFile(this, "singleElementSubscriptionCollection.json");
        doReturn(createOkResponseWithBody(nonEmptySubscriptionsCollectionBody))
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(GET), any(), any());

        val existingSubscriptionBody = readJsonFromFile(this, "subscription1.json")
            .put("Destination", sut.createDestinationUri(serviceId).toString());

        doReturn(createOkResponseWithBody(existingSubscriptionBody))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions/1"), eq(GET), any(), any());

        doReturn(createOkResponse())
            .when(requiredLayer)
            .invoke(eq(serviceId), any(), eq(POST), any(), any());

        sut.run();
        verify(requiredLayer, never()).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }


    @Test
    public void whenRemoteSubscriptionResourceIsBrokenAndMembersFieldIsMissing_subscriptionShouldNotBeRegistered() throws Exception {

        val emptySubscriptionCollectionWithoutMembers = readJsonFromFile(this, "emptySubscriptionsCollection.json").remove("Members");
        doReturn(createOkResponseWithBody(emptySubscriptionCollectionWithoutMembers))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions"), eq(GET), any(), any());

        sut.run();
        verify(requiredLayer, never()).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    @Test
    public void whenDestinationFieldIsMissingOnExistingSubscription_subscriptionShouldNotBeRegistered() throws Exception {

        val singleElementSubscriptionCollection = readJsonFromFile(this, "singleElementSubscriptionCollection.json");
        doReturn(createOkResponseWithBody(singleElementSubscriptionCollection))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions"), eq(GET), any(), any());


        val subscriptionWithoutDestinationBody = readJsonFromFile(this, "subscription1.json").without("Destination");
        doReturn(createOkResponseWithBody(subscriptionWithoutDestinationBody))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions/1"), eq(GET), any(), any());


        sut.run();
        verify(requiredLayer, never()).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    @Test
    public void whenExistingSubscriptionIsNotGettableForSomeReason_subscriptionShouldNotBeRegistered() throws Exception {
        val singleElementSubscriptionCollectionBody = readJsonFromFile(this, "singleElementSubscriptionCollection.json");
        doReturn(createOkResponseWithBody(singleElementSubscriptionCollectionBody))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions"), eq(GET), any(), any());

        doReturn(new Response(null, new HttpHeaders(), HttpStatus.NOT_FOUND))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions/1"), eq(GET), any(), any());

        sut.run();
        verify(requiredLayer, never()).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    @Test
    public void whenSubscriptionsEndpointIsNotGettable_subscriptionShouldNotBeRegistered() {
        doReturn(new Response(null, new HttpHeaders(), HttpStatus.NOT_FOUND))
            .when(requiredLayer)
            .invoke(eq(serviceId), eq("/redfish/v1/EventService/Subscriptions"), eq(GET), any(), any());

        sut.run();
        verify(requiredLayer, never()).invoke(eq(serviceId), any(), eq(POST), any(HttpHeaders.class), any(JsonNode.class));
    }

    private void prepareFindSupportedEventTypesStubbing(EventType... returningEventTypes) {
        val supportedEventTypesArray = stream(returningEventTypes)
            .map(Enum::name)
            .map(TextNode::new)
            .collect(new ArrayNodeCollector());

        doReturn(supportedEventTypesArray).when(sut).findSupportedEventTypes(any());
    }

    private Response createOkResponseWithBody(JsonNode jsonNode) {
        return new Response(jsonNode, new HttpHeaders(), OK);
    }

    private Response createOkResponse() {
        return new Response(null, new HttpHeaders(), OK);
    }

    private void prepareEventsConfigurationStubbing() {
        val fixedProvider = mock(FixedProvider.class);
        when(fixedProvider.getTargetUri()).thenReturn(create("http://localhost:666/events"));

        val receiver = mock(Receiver.class);
        when(receiver.getFixed()).thenReturn(fixedProvider);

        when(eventsConfiguration.getReceiver()).thenReturn(receiver);
    }

    private void prepareSubscriptionDestinationProviderStubbing() {
        when(subscriptionDestinationProvider.provideUriFor(any())).thenReturn(URI.create(""));
    }

    private void prepareRequiredServiceFinderStubbing() {
        when(requiredServiceFinder.getServices()).thenReturn(singleton(serviceId));
    }
}
