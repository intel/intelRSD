/*
 * Copyright (c) 2017-2018 Intel Corporation
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

import com.intel.podm.business.entities.redfish.EventSubscription;
import com.intel.podm.business.entities.types.RedfishEvent;
import com.intel.podm.common.types.Pair;
import com.intel.podm.common.types.events.EventType;

import java.net.URI;
import java.util.Map;
import java.util.Set;
import java.util.function.Function;
import java.util.function.Predicate;
import java.util.stream.Stream;

import static com.intel.podm.common.types.Pair.pairOf;
import static java.util.stream.Collectors.groupingBy;
import static java.util.stream.Collectors.mapping;
import static java.util.stream.Collectors.toSet;

final class EventDispatcher {
    private EventDispatcher() {
    }

    static Map<EventSubscription, Set<RedfishEvent>> dispatch(final Set<RedfishEvent> events, final Set<EventSubscription> subscribers) {
        return events.stream()
            .flatMap(toListOfPairsWith(subscribers))
            .collect(
                groupingBy(
                    Pair::first,
                    mapping(Pair::second, toSet())
                )
            );
    }

    private static Function<? super RedfishEvent, Stream<Pair<EventSubscription, RedfishEvent>>> toListOfPairsWith(Set<EventSubscription> subscribers) {
        return event -> subscribers.stream()
                .filter(byOriginResources(event.getUri()))
                .filter(byEventType(event.getEventType()))
                .map(s -> pairOf(s, event));
    }

    private static Predicate<? super EventSubscription> byEventType(EventType eventType) {
        return subscription -> subscription.getEventTypes().contains(eventType);
    }

    private static Predicate<? super EventSubscription> byOriginResources(URI uri) {
        return subscription -> subscription.getOriginResources().isEmpty()
            || subscription.getOriginResources().contains(uri.toString());
    }
}
