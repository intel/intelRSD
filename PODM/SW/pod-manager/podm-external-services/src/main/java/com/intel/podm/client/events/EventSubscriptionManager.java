/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.events;

import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.client.reader.ResourceSupplier;
import com.intel.podm.client.resources.EventSubscriptionResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.logger.LoggerFactory;
import com.intel.podm.common.types.events.EventType;

import java.net.URI;
import java.util.EnumSet;
import java.util.Optional;

import static com.intel.podm.common.types.EnumeratedType.stringToEnum;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.stream.Collectors.toCollection;

public class EventSubscriptionManager {
    private static final Logger LOGGER = LoggerFactory.getLogger(EventSubscriptionManager.class);
    private static final String EVENT_SERVICE = "/EventService";
    private static final String SUBSCRIPTION_COLLECTION = EVENT_SERVICE + "/Subscriptions";
    private static final String POD_DESCRIPTION_BEGINNING = "Subscribed by PodM with UUID: ";

    private final WebClient webClient;
    private final EventServiceDefinition eventServiceDefinition;
    private final String podManagerSubscriptionDescription;

    public EventSubscriptionManager(WebClient webClient, EventServiceDefinition eventServiceDefinition) {
        this.webClient = webClient;
        this.eventServiceDefinition = eventServiceDefinition;
        this.podManagerSubscriptionDescription = POD_DESCRIPTION_BEGINNING + eventServiceDefinition.getPodmUuid().toString();
    }

    public void subscribeToAllKnownEvents() throws WebClientRequestException {
        EventServiceResource eventServiceResource = getEventService();
        EventSubscribeRequest eventSubscribeRequest = EventSubscribeRequest.newBuilder()
            .destination(eventServiceDefinition.getPodmEventServiceDestinationUri())
            .eventTypes(getAllKnownEventTypesFrom(eventServiceResource))
            .context(podManagerSubscriptionDescription)
            .build();

        webClient.post(createSubscriptionUri(), eventSubscribeRequest);
    }

    private EnumSet<EventType> getAllKnownEventTypesFrom(EventServiceResource eventServiceResource) {
        return eventServiceResource.getEventTypesForSubscription().stream()
            .filter(EventType::isStringRedfishEventType)
            .map(eventTypeString -> stringToEnum(EventType.class, eventTypeString))
            .collect(toCollection(() -> EnumSet.noneOf(EventType.class)));
    }

    public void deleteSubscription(EventSubscriptionResource eventSubscriptionResource) throws WebClientRequestException {
        requiresNonNull(eventSubscriptionResource, "eventSubscriptionResource");
        webClient.delete(eventSubscriptionResource.getUri());
    }

    public Optional<EventSubscriptionResource> fetchPodmSubscription() throws WebClientRequestException {
        EventServiceResource eventServiceResource = getEventService();
        for (ResourceSupplier eventSubscription : eventServiceResource.getSubscriptions()) {
            EventSubscriptionResource eventSubscriptionResource = (EventSubscriptionResource) eventSubscription.get();
            boolean wasSubscribedByCurrentPodManager = podManagerSubscriptionDescription.equals(eventSubscriptionResource.getContext());
            if (wasSubscribedByCurrentPodManager) {
                return of(eventSubscriptionResource);
            }
        }
        return empty();
    }

    private EventServiceResource getEventService() throws WebClientRequestException {
        return (EventServiceResource) webClient.get(createEventServiceUri());
    }

    private URI createEventServiceUri() {
        return URI.create(webClient.getBaseUri() + EVENT_SERVICE);
    }

    private URI createSubscriptionUri() {
        return URI.create(webClient.getBaseUri() + SUBSCRIPTION_COLLECTION);
    }
}
