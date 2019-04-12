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

package com.intel.rsd.resourcemanager.runner.events;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.NullNode;
import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.dto.events.SubscriptionRequest;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.runner.events.configuration.EventsConfiguration;
import com.intel.rsd.resourcemanager.runner.requiredlayer.RequiredLayer;
import com.intel.rsd.resourcemanager.runner.requiredlayer.RequiredServiceFinder;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.web.client.RestClientException;

import java.io.Serializable;
import java.net.URI;
import java.util.Set;
import java.util.stream.StreamSupport;

import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static java.util.stream.Collectors.toSet;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;
import static org.springframework.web.util.UriComponentsBuilder.fromUri;

@Slf4j
@SpringAware
class EventSubscriptionTask implements Runnable, Serializable {

    private final ServiceId serviceId;

    @Autowired
    private transient RequiredLayer requiredLayer;

    @Autowired
    private transient RequiredServiceFinder requiredServiceFinder;

    @Autowired
    private transient SubscriptionDestinationProvider subscriptionDestinationProvider;

    @Autowired
    private transient EventsConfiguration eventsConfiguration;

    EventSubscriptionTask(ServiceId serviceId) {
        this.serviceId = serviceId;
    }

    @Override
    public void run() {
        try {
            if (subscriptionIsMissing(serviceId)) {
                val subscriptionRequest = prepareSubscriptionRequest(serviceId);
                val response = requiredLayer.invoke(serviceId, "/redfish/v1/EventService/Subscriptions", POST, new HttpHeaders(), subscriptionRequest);
                if (!response.getHttpStatus().is2xxSuccessful()) {
                    log.warn(
                        "Cannot register subscription on service: "
                            + serviceId + ": status: "
                            + response.getHttpStatus()
                            + ", message: "
                            + response.getBody()
                    );
                } else {
                    log.info("Successfully subscribed for events on service: {} using request: {}", serviceId, subscriptionRequest);
                }
            }

        } catch (RestClientException | IllegalStateException | SubscriptionDestinationProviderException ex) {
            log.warn("Subscription registration for service({}) cannot be continued: {}", serviceId, ex.getMessage());
        }
    }

    private boolean subscriptionIsMissing(ServiceId serviceId) {
        val subscriptions = findRegisteredSubscriptions(serviceId);
        if (subscriptions.size() == 0) {
            return true;
        }
        val expectedDestinationUri = createDestinationUri(serviceId);
        for (JsonNode member : subscriptions) {
            val registeredDestination = findRegisteredDestination(serviceId, member);
            if (expectedDestinationUri.toString().equals(registeredDestination.asText())) {
                return false;
            }
        }
        return true;
    }

    private JsonNode prepareSubscriptionRequest(ServiceId serviceId) {
        val subscriptionRequest = new SubscriptionRequest();
        subscriptionRequest.setName("PODM");
        subscriptionRequest.setProtocol("Redfish");
        subscriptionRequest.setDestination(createDestinationUri(serviceId));
        subscriptionRequest.setEventTypes(determineSupportedEventTypes(serviceId));
        subscriptionRequest.setContext("Pod Manager subscription");
        return objectToJsonNode(subscriptionRequest);
    }

    private JsonNode findRegisteredSubscriptions(ServiceId serviceId) {
        val getSubscriptionsResponse = requiredLayer.invoke(serviceId, "/redfish/v1/EventService/Subscriptions", GET);
        if (getSubscriptionsResponse.getHttpStatus().is2xxSuccessful()) {
            val members = getSubscriptionsResponse.getBody().get("Members");
            if (members != null) {
                return members;
            }
            throw new IllegalStateException("/Members/ field does not exists on subscription collection: " + getSubscriptionsResponse.getBody());
        }
        throw new IllegalStateException("Subscription collection is not gettable, response status: " + getSubscriptionsResponse.getHttpStatus());
    }

    private Set<EventType> determineSupportedEventTypes(ServiceId serviceId) {
        val supportedEventTypes = findSupportedEventTypes(serviceId);
        if (supportedEventTypes.size() == 0) {
            throw new IllegalStateException(
                String.format("Service(%s) does not expose information about supported event types(EventTypesForSubscription)", serviceId));
        }

        return StreamSupport.stream(supportedEventTypes.spliterator(), false)
            .map(jsonEventType -> EventType.valueOf(jsonEventType.asText()))
            .collect(toSet());
    }

    private JsonNode findRegisteredDestination(ServiceId serviceId, JsonNode member) {
        val subscriptionUri = member.get("@odata.id");
        if (subscriptionUri == null) {
            throw new IllegalStateException("Given subscription's member does not specify /@odata.id/ field: " + member);
        }

        val subscriptionResponse = requiredLayer.invoke(serviceId, subscriptionUri.asText(), GET);
        if (!subscriptionResponse.getHttpStatus().is2xxSuccessful()) {
            throw new IllegalStateException("Cannot get subscription: " + subscriptionUri + ", response status: " + subscriptionResponse.getHttpStatus());
        }

        val destination = subscriptionResponse.getBody().get("Destination");
        if (destination == null) {
            throw new IllegalStateException("Subscription does not specify /Destination/ field: " + subscriptionResponse.getBody());
        }
        return destination;
    }

    JsonNode findSupportedEventTypes(ServiceId serviceId) {
        val response = requiredLayer.invoke(serviceId, "/redfish/v1/EventService", GET);
        if (response.getHttpStatus().is2xxSuccessful()) {
            return response.getBody().at("/EventTypesForSubscription");
        } else {
            return NullNode.getInstance();
        }
    }

    URI createDestinationUri(ServiceId destinationServiceId) {
        val serviceHost = requiredServiceFinder.findServiceHost(destinationServiceId);

        return fromUri(subscriptionDestinationProvider.provideUriFor(serviceHost))
            .pathSegment(eventsConfiguration.getReceiver().getEndpoint(), destinationServiceId.getValue())
            .build()
            .toUri();
    }
}
