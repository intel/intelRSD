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
import io.reactivex.Observable;
import io.reactivex.functions.Consumer;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import static io.reactivex.Observable.just;

@Component
public class EventHandler implements Consumer<Event> {
    private final EventDispatcher eventDispatcher;
    private final EventPublisher eventPublisher;
    private final Schedulers schedulers;

    @Autowired
    public EventHandler(EventDispatcher eventDispatcher, EventPublisher eventPublisher, Schedulers schedulers) {
        this.eventDispatcher = eventDispatcher;
        this.eventPublisher = eventPublisher;
        this.schedulers = schedulers;
    }

    @Override
    public void accept(Event event) {
        just(event)
            .compose(schedulers.applySchedulers())
            .map(eventDispatcher::dispatch)
            .flatMap(Observable::fromIterable)
            .subscribe(eventPublisher::publish);
    }
}
