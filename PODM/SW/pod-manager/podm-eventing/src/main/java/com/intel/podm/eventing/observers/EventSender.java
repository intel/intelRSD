/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.eventing.observers;

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.types.RedfishEvent;
import com.intel.podm.client.WebClient;
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.WebClientRequestException;
import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.eventing.json.EventArrayJson;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.inject.Named;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.ScheduledExecutorService;

import static com.intel.podm.common.enterprise.utils.beans.JndiNames.EVENT_SUBSCRIPTION_TASK_EXECUTOR;
import static java.util.concurrent.TimeUnit.SECONDS;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
public class EventSender extends DefaultManagedTask implements Runnable {
    @Inject
    private Logger logger;

    @Inject
    private GenericDao genericDao;

    @Inject
    @Named(EVENT_SUBSCRIPTION_TASK_EXECUTOR)
    private ScheduledExecutorService eventSubscriptionTaskExecutor;

    @Inject
    private WebClientBuilder webClientBuilder;

    private long deliveryRetryAttempts;
    private long deliveryRetryIntervalSeconds;
    private Id eventSubscriptionId;
    private Set<RedfishEvent> events;

    private static EventArrayJson createEventArray(Set<RedfishEvent> redfishEvent, String subscriptionContext) {
        EventArrayJson eventArray = new EventArrayJson();
        eventArray.description = "Redfish event";
        eventArray.id = "1";
        eventArray.name = "Event array";
        eventArray.odataContext = URI.create("/redfish/v1/$metadata#EventService/Members/Events/1");
        eventArray.oDataId = URI.create("/redfish/v1/EventService/Events/1");
        eventArray.odataType = "#EventService.v1_2_0.Event";
        eventArray.events.addAll(redfishEvent.stream().map(EventSender::toEventJson).collect(toList()));
        eventArray.context = subscriptionContext;
        return eventArray;
    }

    private static EventArrayJson.EventJson toEventJson(RedfishEvent redfishEvent) {
        EventArrayJson.EventJson eventJson = new EventArrayJson.EventJson();
        eventJson.eventType = redfishEvent.getEventType();
        eventJson.messageId = "1";
        eventJson.originOfCondition = redfishEvent.getUri().toString();
        return eventJson;
    }

    public void setEventSubscriptionId(Id eventSubscriptionId) {
        this.eventSubscriptionId = eventSubscriptionId;
    }

    public void setEvents(Set<RedfishEvent> events) {
        this.events = events;
    }

    public void setDeliveryRetryAttempts(long deliveryRetryAttempts) {
        this.deliveryRetryAttempts = deliveryRetryAttempts;
    }

    public void setDeliveryRetryIntervalSeconds(long deliveryRetryIntervalSeconds) {
        this.deliveryRetryIntervalSeconds = deliveryRetryIntervalSeconds;
    }

    @Override
    @Transactional(REQUIRED)
    public void run() {
        Optional<EventSubscription> eventSubscription = genericDao.tryFind(EventSubscription.class, eventSubscriptionId);
        if (!eventSubscription.isPresent()) {
            logger.w("Event subscription with id: {} is no longer available.", eventSubscriptionId);
            return;
        }

        if (!trySendNotification(eventSubscription.get(), events)) {
            retryOrRemoveSubscription(eventSubscription.get());
        }
    }

    private void retryOrRemoveSubscription(EventSubscription eventSubscription) {
        if (deliveryRetryAttempts > 0) {
            deliveryRetryAttempts--;
            eventSubscriptionTaskExecutor.schedule(this, deliveryRetryIntervalSeconds, SECONDS);
        } else {
            logger.d("Removing subscription: {}", eventSubscription.getDestination());
            genericDao.remove(eventSubscription);
        }
    }

    private boolean trySendNotification(EventSubscription subscription, Set<RedfishEvent> events) {
        logger.d("Sending {} to {}", events, subscription.getDestination());
        URI destinationUri = URI.create(subscription.getDestination());
        try (WebClient client = webClientBuilder.newInstance(destinationUri).build()) {
            EventArrayJson eventArray = createEventArray(events, subscription.getSubscriptionContext());
            client.post(URI.create(destinationUri.getPath()), eventArray);
            return true;
        } catch (WebClientRequestException e) {
            logger.d("Could not send event to destination: {}. Exception: {}", destinationUri, e.getMessage());
            return false;
        }
    }
}
