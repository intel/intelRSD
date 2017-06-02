/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.client.resources;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.api.resources.EventSubscriptionResource;
import com.intel.podm.common.types.events.EventType;

import java.net.URI;
import java.util.EnumSet;

import static java.net.URI.create;
import static java.util.EnumSet.noneOf;

@OdataTypes({
    "#EventService" + OdataTypes.VERSION_PATTERN + "EventDestination",
    "#EventDestination" + OdataTypes.VERSION_PATTERN + "EventDestination",
    "#EventServiceSubscription" + OdataTypes.VERSION_PATTERN + "EventServiceSubscription"
})
public class EventSubscriptionResourceImpl extends ExternalServiceResourceImpl implements EventSubscriptionResource {
    @JsonProperty("Destination")
    private String destination;

    @JsonProperty("EventTypes")
    private EnumSet<EventType> eventTypes;

    @JsonProperty("Context")
    private String context;

    @Override
    public URI getDestination() {
        return create(destination);
    }

    @Override
    public EnumSet<EventType> getEventTypes() {
        if (eventTypes == null) {
            return noneOf(EventType.class);
        }
        return eventTypes;
    }

    @Override
    public String getContext() {
        return context;
    }
}
