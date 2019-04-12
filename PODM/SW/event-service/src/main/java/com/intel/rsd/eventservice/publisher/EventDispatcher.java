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

package com.intel.rsd.eventservice.publisher;

import com.intel.rsd.dto.ODataId;
import com.intel.rsd.dto.events.Event;
import com.intel.rsd.dto.events.EventRecord;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.eventservice.Subscription;
import com.intel.rsd.eventservice.SubscriptionService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.AbstractMap.SimpleEntry;
import java.util.Collection;
import java.util.List;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Collector;
import java.util.stream.Collectors;
import java.util.stream.Stream;

import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.mapping;
import static java.util.stream.Collectors.toList;

@Component
@Slf4j
class EventDispatcher {
    private final SubscriptionService subscriptionService;

    @Autowired
    EventDispatcher(SubscriptionService subscriptionService) {
        this.subscriptionService = subscriptionService;
    }

    List<Envelope> dispatch(final Event event) {
        log.info("Dispatching event: {}", event.getId());
        return event.getEventRecords().stream()
            .flatMap(toListOfPairsWith(subscriptionService.getAllSubscriptions()))
            .collect(groupingBy(SimpleEntry::getKey, redfishEventCollector(event)))
            .entrySet().stream().map(e -> new Envelope(e.getKey(), e.getValue())).collect(toList());
    }

    private static Function<EventRecord, Stream<SimpleEntry<Subscription, EventRecord>>> toListOfPairsWith(Collection<Subscription> subscribers) {
        return event -> subscribers.stream()
            .filter(byOriginResources(event.getOriginOfCondition()))
            .filter(byEventType(event.getEventType()))
            .map(s -> new SimpleEntry<>(s, event));
    }

    private static Predicate<Subscription> byOriginResources(ODataId oDataId) {
        return subscription -> {
            Collection<ODataId> originResources = subscription.getSubscriptionRequest().getOriginResources();
            return originResources == null || originResources.isEmpty() || originResources.stream().anyMatch(odid -> odid.equals(oDataId));
        };
    }

    private static Predicate<Subscription> byEventType(EventType eventType) {
        return subscription -> subscription.getSubscriptionRequest().getEventTypes().contains(eventType);
    }

    private Collector<SimpleEntry<Subscription, EventRecord>, ?, Event> redfishEventCollector(Event event) {
        return mapping(
            SimpleEntry::getValue,
            Collectors.collectingAndThen(
                toList(),
                events -> createEvent(events, event.getId())
            )
        );
    }

    private Event createEvent(List<EventRecord> events, String eventId) {
        Event event = new Event();
        event.setEventRecords(events);
        event.setId(eventId);

        return event;
    }
}
