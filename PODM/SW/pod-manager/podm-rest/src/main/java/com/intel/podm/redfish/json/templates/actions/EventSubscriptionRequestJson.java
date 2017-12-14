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

package com.intel.podm.redfish.json.templates.actions;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.business.services.redfish.requests.EventSubscriptionRequest;
import com.intel.podm.common.types.events.EventType;

import java.net.URI;
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;

import static java.util.EnumSet.noneOf;

public class EventSubscriptionRequestJson implements EventSubscriptionRequest {
    @JsonProperty("Name")
    private String name;

    @JsonProperty("Destination")
    private URI destination;

    @JsonProperty("Description")
    private String description;

    @JsonProperty("EventTypes")
    private EnumSet<EventType> eventTypes = noneOf(EventType.class);

    @JsonProperty("Context")
    private String subscriptionContext;

    @JsonProperty("Protocol")
    private String protocol;

    @JsonProperty("OriginResources")
    private List<ODataId> originResources = new ArrayList<>();

    @Override
    public String getDescription() {
        return description;
    }

    public String getName() {
        return name;
    }

    public URI getDestination() {
        return destination;
    }

    public EnumSet<EventType> getEventTypes() {
        return eventTypes;
    }

    public String getSubscriptionContext() {
        return subscriptionContext;
    }

    public String getProtocol() {
        return protocol;
    }

    @Override
    public List<ODataId> getOriginResources() {
        return originResources;
    }
}
