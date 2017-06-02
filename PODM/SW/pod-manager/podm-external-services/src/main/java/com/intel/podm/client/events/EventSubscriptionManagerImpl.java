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

import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.events.EventServiceDefinition;
import com.intel.podm.client.api.events.EventSubscriptionManager;
import com.intel.podm.client.api.reader.ResourceSupplier;
import com.intel.podm.client.api.resources.EventServiceResource;
import com.intel.podm.client.api.resources.EventSubscriptionResource;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.events.EventType;
import org.apache.commons.lang.StringUtils;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.net.URI;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.EnumeratedType.stringToEnum;
import static java.net.URI.create;
import static java.util.EnumSet.copyOf;
import static java.util.stream.Collectors.toSet;

@Dependent
public class EventSubscriptionManagerImpl implements EventSubscriptionManager {
    private static final String EVENT_SERVICE = "/EventService";
    private static final String SUBSCRIPTION_COLLECTION = EVENT_SERVICE + "/Subscriptions";
    private static final String POD_DESCRIPTION_BEGINNING = "Subscribed by PodM with UUID: ";

    @Inject
    private Logger logger;

    private WebClient webClient;
    private EventActions eventActions;
    private EventServiceDefinition eventServiceDefinition;

    public void setWebClient(WebClient webClient) {
        this.webClient = webClient;
        eventActions = new EventActions(webClient);
    }

    public void setEventServiceDefinition(EventServiceDefinition eventServiceDefinition) {
        this.eventServiceDefinition = eventServiceDefinition;
    }

    @Override
    public boolean subscribeIfNotAlreadySubscribed() throws ExternalServiceApiReaderException, ExternalServiceApiActionException {
        String podDescription = POD_DESCRIPTION_BEGINNING + eventServiceDefinition.getPodmUuid().toString();
        EventServiceResource eventServiceResource = getEventService();

        if (!getSubscriptions(podDescription, eventServiceResource).isEmpty()) {
            return false;
        }

        subscribe(podDescription, eventServiceResource);
        return true;
    }

    private List<EventSubscriptionResource> getSubscriptions(String podDescription, EventServiceResource eventServiceResource)
            throws ExternalServiceApiReaderException, ExternalServiceApiActionException {
        List<EventSubscriptionResource> subscriptions = new ArrayList<>();

        for (ResourceSupplier eventSubscription : eventServiceResource.getSubscriptions()) {
            try {
                EventSubscriptionResource eventSubscriptionResource = (EventSubscriptionResource) eventSubscription.get();
                boolean wasSubscribedByCurrentPodManager = podDescription.equals(eventSubscriptionResource.getContext());
                if (wasSubscribedByCurrentPodManager) {
                    if (subscriptionHasCurrentDestinationUri(eventSubscriptionResource)) {
                        subscriptions.add(eventSubscriptionResource);
                    } else {
                        eventActions.deleteSubscription(eventSubscriptionResource.getUri());
                    }
                }
            } catch (ExternalServiceApiReaderException e) {
                logger.w("Could not read event subscription: {}, error: {}", e.getResourceUri(), e.getErrorResponse(), e);
            }
        }

        return subscriptions;
    }

    private boolean subscriptionHasCurrentDestinationUri(EventSubscriptionResource eventSubscriptionResource) {
        return eventServiceDefinition.getPodmEventServiceDestinationUri().equals(eventSubscriptionResource.getDestination());
    }

    private EventServiceResource getEventService() throws ExternalServiceApiReaderException {
        return (EventServiceResource) webClient.get(createEventServiceUri());
    }

    private void subscribe(String podDescription, EventServiceResource eventServiceResource)
            throws ExternalServiceApiActionException, ExternalServiceApiReaderException {
        EventSubscribeRequest eventSubscribeRequest = EventSubscribeRequest
                .newBuilder()
                .destination(eventServiceDefinition.getPodmEventServiceDestinationUri())
                .eventTypes(copyOf(eventServiceResource.getEventTypesForSubscription().stream()
                        .filter(EventType::isStringRedfishEventType)
                        .filter(StringUtils::isNotBlank)
                        .map(eventTypeString -> stringToEnum(EventType.class, eventTypeString))
                        .collect(toSet())))
                .context(podDescription)
                .build();

        eventActions.subscribe(createSubscriptionUri(), eventSubscribeRequest);
    }

    private URI createEventServiceUri() {
        return create(webClient.getBaseUri() + EVENT_SERVICE);
    }

    private URI createSubscriptionUri() {
        return create(webClient.getBaseUri() + SUBSCRIPTION_COLLECTION);
    }

    @Override
    public void close() {
        webClient.close();
    }
}
