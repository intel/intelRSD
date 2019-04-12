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

package com.intel.rsd.eventservice.utils;

import com.intel.rsd.dto.events.Event;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import javax.validation.Valid;
import java.util.concurrent.LinkedBlockingDeque;

@RestController
@RequestMapping("/eventCatcher")
public class EventCatcher {
    private LinkedBlockingDeque<Event> queue = new LinkedBlockingDeque<>();

    @PostMapping
    public void receiveEvent(@Valid @RequestBody Event event) {
        queue.offer(event);
    }

    public LinkedBlockingDeque<Event> getQueue() {
        return queue;
    }
}
