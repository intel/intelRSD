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
import com.intel.podm.client.WebClientBuilder;
import com.intel.podm.client.api.ExternalServiceApiActionException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.enterprise.utils.tasks.DefaultManagedTask;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.EventsConfig;
import com.intel.podm.eventing.json.EventArrayJson;

import javax.ejb.Singleton;
import javax.enterprise.context.Dependent;
import javax.enterprise.event.Observes;
import javax.inject.Inject;
import javax.inject.Named;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.Set;
import java.util.concurrent.ScheduledExecutorService;
import java.util.function.Function;

import static com.google.common.collect.Sets.newHashSet;
import static com.intel.podm.eventing.observers.EventDispatcher.dispatch;
import static java.util.concurrent.TimeUnit.SECONDS;
import static java.util.stream.Collectors.toList;
import static javax.transaction.Transactional.TxType.REQUIRED;
import static javax.transaction.Transactional.TxType.REQUIRES_NEW;

@Singleton
public class RedfishEventObserver {

    @Inject
    private GenericDao genericDao;

    @Inject
    private Logger logger;

    @Inject
    @Named("EventsExecutor")
    private ScheduledExecutorService eventSubscriptionTaskExecutor;

    @Inject
    private EventSenderFactory eventSenderFactory;

    @Transactional(REQUIRES_NEW)
    public void observe(@Observes List<RedfishEvent> events) {
        Set<RedfishEvent> occurredEvents = newHashSet(events);
        Set<EventSubscription> allSubscribers = newHashSet(genericDao.findAll(EventSubscription.class));

        Map<EventSubscription, Set<RedfishEvent>> dispatched = dispatch(occurredEvents, allSubscribers);

        for (Map.Entry<EventSubscription, Set<RedfishEvent>> entry : dispatched.entrySet()) {
            EventSender eventSender = eventSenderFactory.createEventSender(entry.getKey().getId(), entry.getValue());
            eventSubscriptionTaskExecutor.schedule(eventSender, 0, SECONDS);
        }
    }

    @Dependent
    static class EventSenderFactory {
        @Inject
        private BeanFactory beanFactory;

        @Inject
        @Config
        private Holder<EventsConfig> eventsConfig;

        EventSender createEventSender(Id subscriptionId, Set<RedfishEvent> events) {
            EventSender eventSender = beanFactory.create(EventSender.class);
            eventSender.setEventSubscriptionId(subscriptionId);
            eventSender.setEvents(events);
            EventsConfig.NorthboundConfiguration northboundConfiguration = eventsConfig.get().getNorthboundConfiguration();
            eventSender.setDeliveryRetryAttempts(northboundConfiguration.getDeliveryRetryAttempts());
            eventSender.setDeliveryRetryIntervalSeconds(northboundConfiguration.getDeliveryRetryIntervalSeconds());
            return eventSender;
        }
    }

    @Dependent
    static class EventSender extends DefaultManagedTask implements Runnable {
        @Inject
        private Logger logger;

        @Inject
        private GenericDao genericDao;

        @Inject
        @Named("EventsExecutor")
        private ScheduledExecutorService eventSubscriptionTaskExecutor;

        @Inject
        private WebClientBuilder webClientBuilder;

        private long deliveryRetryAttempts;
        private long deliveryRetryIntervalSeconds;
        private Id eventSubscriptionId;
        private Set<RedfishEvent> events;

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
            } catch (ExternalServiceApiActionException e) {
                logger.d("Could not send event to destination: {}. Exception: {}", destinationUri, e.getMessage());
                return false;
            }
        }

        private static EventArrayJson createEventArray(Set<RedfishEvent> redfishEvent, String subscriptionContext) {
            EventArrayJson eventArray = new EventArrayJson();
            eventArray.description = "Redfish event";
            eventArray.id = "1";
            eventArray.name = "Event array";
            eventArray.odataContext = URI.create("/redfish/v1/$metadata#EventService/Members/Events/1");
            eventArray.oDataId = URI.create("/redfish/v1/EventService/Events/1");
            eventArray.odataType = "#EventService.v1_0_0.Event";
            eventArray.events.addAll(redfishEvent.stream().map(toEventJson(subscriptionContext)).collect(toList()));
            return eventArray;
        }

        private static Function<RedfishEvent, EventArrayJson.EventJson> toEventJson(String subscriptionContext) {
            return event -> {
                EventArrayJson.EventJson eventJson = new EventArrayJson.EventJson();
                eventJson.eventType = event.getEventType();
                eventJson.messageId = "1";
                eventJson.context = subscriptionContext;
                eventJson.originOfCondition = event.getUri().toString();
                return eventJson;
            };
        }
    }
}
