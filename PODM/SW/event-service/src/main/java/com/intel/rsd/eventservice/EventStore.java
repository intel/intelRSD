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

package com.intel.rsd.eventservice;

import com.intel.rsd.dto.events.Event;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicLong;

@Component
public class EventStore {
    private Collection<EventStoreListener> eventStoreListeners = new ArrayList<>();
    private AtomicLong lastId = new AtomicLong();
    private Map<Object, Event> events = Collections.synchronizedMap(new LinkedHashMap<Object, Event>() {
        @Override
        protected boolean removeEldestEntry(Map.Entry entry) {
            return size() > 1000;
        }
    });

    public void store(Event event) {
        String id = generateId();
        event.setId(id);
        events.put(id, event);
        eventStoreListeners.forEach(o -> o.onEventStored(event));
    }

    public void registerObserver(EventStoreListener eventStoreListener) {
        this.eventStoreListeners.add(eventStoreListener);
    }

    private String generateId() {
        return String.valueOf(lastId.incrementAndGet());
    }

    public interface EventStoreListener {
        void onEventStored(Event event);
    }
}
