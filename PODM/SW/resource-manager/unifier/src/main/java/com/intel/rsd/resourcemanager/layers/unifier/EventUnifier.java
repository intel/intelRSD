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

package com.intel.rsd.resourcemanager.layers.unifier;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.EventHandler;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.unifier.unification.GrandUnifier;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

@Component
public class EventUnifier implements EventHandler {

    private final UnificationLayer unificationLayer;
    private final GrandUnifier unifier;

    private EventHandler next;

    @Autowired
    public EventUnifier(UnificationLayer unificationLayer, GrandUnifier unifier) {
        this.unificationLayer = unificationLayer;
        this.unifier = unifier;
    }

    @Override
    public String id() {
        return "Unifier";
    }

    @Override
    public void setNext(EventHandler next) {
        this.next = next;
    }

    @Override
    public void onEvent(ServiceId serviceId, JsonNode event) {
        val unified = unifier.unify(event, unificationLayer.createLoadingResolver(serviceId));
        next.onEvent(serviceId, unified);
    }
}
