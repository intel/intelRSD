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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Pair;
import com.intel.podm.common.types.events.EventType;

import javax.enterprise.context.ApplicationScoped;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BiFunction;
import java.util.function.Function;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static com.intel.podm.common.types.Pair.pairOf;
import static java.lang.ThreadLocal.withInitial;
import static java.util.stream.Collectors.toList;

@ApplicationScoped
public class EntityEventCollector {
    private static final Logger LOGGER = getLogger(EntityEventCollector.class);

    private final ThreadLocal<List<Pair<Entity, EventType>>> eventsContainer = withInitial(ArrayList::new);

    public void add(Entity eventableEntity, EventType eventType) {
        LOGGER.t("Registering event {} for: {}", eventType, eventableEntity);
        eventsContainer.get().add(pairOf(eventableEntity, eventType));
    }

    public void add(Entity eventableEntity, List<EventType> eventTypes) {
        LOGGER.t("Registering events {} for: {}", eventTypes, eventableEntity);
        eventTypes.forEach(eventType -> eventsContainer.get().add(pairOf(eventableEntity, eventType)));
    }

    public void convertEventTypes(BiFunction<Entity, EventType, EventType> converter) {
        List<Pair<Entity, EventType>> converted = eventsContainer.get()
            .stream()
            .map(pair -> pairOf(pair.first(), converter.apply(pair.first(), pair.second())))
            .collect(toList());

        eventsContainer.set(converted);
    }

    public <T> T convertThenEvict(Function<List<Pair<Entity, EventType>>, T> converter) {
        try {
            return converter.apply(eventsContainer.get());
        } finally {
            eventsContainer.remove();
        }
    }

    public void reset() {
        eventsContainer.remove();
    }
}
