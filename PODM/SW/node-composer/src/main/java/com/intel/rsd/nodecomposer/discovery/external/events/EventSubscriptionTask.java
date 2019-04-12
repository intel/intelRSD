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

package com.intel.rsd.nodecomposer.discovery.external.events;

import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.dto.events.SubscriptionRequest;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.externalservices.WebClient;
import com.intel.rsd.nodecomposer.externalservices.WebClientBuilder;
import com.intel.rsd.nodecomposer.externalservices.WebClientRequestException;
import com.intel.rsd.nodecomposer.externalservices.resources.MembersListResource;
import com.intel.rsd.nodecomposer.externalservices.resources.redfish.EventSubscriptionResource;
import com.intel.rsd.services.common.hazelcast.ScheduledWithFixedDelayOnLeaderTask;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.cloud.client.ServiceInstance;
import org.springframework.cloud.client.loadbalancer.LoadBalancerClient;
import org.springframework.scheduling.TaskScheduler;
import org.springframework.stereotype.Component;

import java.net.URI;
import java.time.Duration;
import java.util.EnumSet;
import java.util.Objects;
import java.util.UUID;

import static com.intel.rsd.dto.events.EventType.ResourceAdded;
import static java.util.Collections.emptySet;

@Slf4j
@Component
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class EventSubscriptionTask extends ScheduledWithFixedDelayOnLeaderTask {
    private static final URI SUBSCRIPTIONS_URI = URI.create("/redfish/v1/EventService/Subscriptions");
    private static final EnumSet<EventType> SUBSCRIPTION_EVENT_TYPES = EnumSet.of(ResourceAdded);
    private static final String CONTEXT = UUID.randomUUID().toString();

    private LoadBalancerClient loadBalancerClient;
    private WebClientBuilder webClientBuilder;
    private String serverAddress;
    private int serverPort;
    private final int taskDelaySeconds;

    public EventSubscriptionTask(TaskScheduler taskScheduler, LoadBalancerClient loadBalancerClient, WebClientBuilder webClientBuilder,
                                 EventSubscriptionTaskConfig eventSubscriptionTaskConfig) {
        super(taskScheduler);
        this.loadBalancerClient = loadBalancerClient;
        this.webClientBuilder = webClientBuilder;
        this.serverAddress = eventSubscriptionTaskConfig.getServerAddress();
        this.serverPort = eventSubscriptionTaskConfig.getServerPort();
        this.taskDelaySeconds = eventSubscriptionTaskConfig.getTaskDelaySeconds();
    }

    @Override
    public void run() {
        URI serviceUri = getEventServiceUri();
        if (serviceUri == null) {
            log.debug("No event-service available.");
            return;
        }

        try (val webClient = webClientBuilder.newSimpleInstance(serviceUri).build()) {
            val subscription = getExistingSubscriptionOrNull(webClient);

            if (subscription == null) {
                createNewSubscription(webClient);
            } else if (!subscriptionIsValid(subscription)) {
                deleteSubscription(webClient, subscription);
                createNewSubscription(webClient);
            }
        } catch (WebClientRequestException e) {
            log.error("Unable to update subscription", e);
        }
    }

    private void deleteSubscription(WebClient webClient, EventSubscriptionResource subscription) throws WebClientRequestException {
        webClient.delete(subscription.getUri());
    }

    private URI getEventServiceUri() {
        ServiceInstance serviceInstance = loadBalancerClient.choose("event-service");

        if (serviceInstance == null) {
            return null;
        }

        return serviceInstance.getUri();
    }

    private URI getDestination() {
        return URI.create(String.format("http://%s:%s/redfish/v1/EventReceiver", serverAddress, serverPort));
    }

    private void createNewSubscription(WebClient webClient) throws WebClientRequestException {
        val subscriptionRequest = new SubscriptionRequest();
        subscriptionRequest.setEventTypes(SUBSCRIPTION_EVENT_TYPES);
        subscriptionRequest.setDestination(getDestination());
        subscriptionRequest.setName("Node Composer subscription");
        subscriptionRequest.setContext(CONTEXT);
        subscriptionRequest.setOriginResources(emptySet()); //why this cant be null?
        try {
            webClient.post(SUBSCRIPTIONS_URI, subscriptionRequest);
            log.info("Created new subscription on {} using {}", webClient.getBaseUri(), subscriptionRequest);
        } catch (WebClientRequestException e) {
            log.warn("Could not create event subscription on {} due to {}", e.getResourceUri(), e.getMessage());
            throw e;
        }
    }

    private EventSubscriptionResource getExistingSubscriptionOrNull(WebClient webClient) throws WebClientRequestException {
        MembersListResource membersListResource = (MembersListResource) webClient.get(SUBSCRIPTIONS_URI);
        for (ODataId subscriptionOdataId : membersListResource.getMembers()) {
            val eventSubscriptionResource = (EventSubscriptionResource) webClient.get(subscriptionOdataId.toUri());
            if (eventSubscriptionResource.getDestination().equals(getDestination())) {
                return eventSubscriptionResource;
            }
        }

        return null;
    }

    private boolean subscriptionIsValid(EventSubscriptionResource eventSubscriptionResource) {
        return eventSubscriptionResource != null
            && Objects.equals(eventSubscriptionResource.getContext(), CONTEXT)
            && Objects.equals(eventSubscriptionResource.getEventTypes(), SUBSCRIPTION_EVENT_TYPES);
    }

    @Override
    protected Duration getFixedDelay() {
        return Duration.ofSeconds(taskDelaySeconds);
    }

    @Override
    public String toString() {
        return "event-subscription-task";
    }
}
