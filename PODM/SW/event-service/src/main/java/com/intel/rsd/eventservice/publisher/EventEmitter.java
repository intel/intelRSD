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

import com.intel.rsd.dto.events.Event;
import com.intel.rsd.eventservice.EventStore;
import io.reactivex.ObservableEmitter;
import io.reactivex.ObservableOnSubscribe;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
@Slf4j
class EventEmitter implements ObservableOnSubscribe<Event> {
    private final EventStore eventStore;

    @Autowired
    EventEmitter(EventStore eventStore) {
        this.eventStore = eventStore;
    }

    @Override
    public void subscribe(ObservableEmitter<Event> emitter) {
        eventStore.registerObserver(event -> {
            log.info("Event Stored: {}", event.getId());
            emitter.onNext(event);
        });
    }
}
