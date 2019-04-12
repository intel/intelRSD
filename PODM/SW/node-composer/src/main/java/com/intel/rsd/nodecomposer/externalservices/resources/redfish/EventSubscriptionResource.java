/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.rsd.dto.events.EventType;
import com.intel.rsd.nodecomposer.externalservices.OdataTypes;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;
import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResourceImpl;

import java.net.URI;
import java.util.EnumSet;

import static com.intel.rsd.redfish.ODataTypeVersions.VERSION_PATTERN;
import static java.util.EnumSet.noneOf;

@OdataTypes({
    "#EventDestination" + VERSION_PATTERN + "EventDestination",
})
public class EventSubscriptionResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Destination")
    private String destination;

    @JsonProperty("EventTypes")
    private EnumSet<EventType> eventTypes;

    @JsonProperty("Context")
    private String context;

    public URI getDestination() {
        return URI.create(destination);
    }

    public EnumSet<EventType> getEventTypes() {
        if (eventTypes == null) {
            return noneOf(EventType.class);
        }
        return eventTypes;
    }

    public String getContext() {
        return context;
    }
}
