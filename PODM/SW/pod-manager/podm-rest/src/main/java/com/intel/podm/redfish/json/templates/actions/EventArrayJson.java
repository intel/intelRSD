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

package com.intel.podm.redfish.json.templates.actions;


import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.common.types.redfish.RedfishEventArray;
import com.intel.podm.business.services.redfish.odataid.ODataId;

import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;

import static java.util.Optional.ofNullable;

public class EventArrayJson implements RedfishEventArray {
    @JsonProperty("@odata.type")
    private String oDataType;

    @JsonProperty("@odata.context")
    private URI oDataContext;

    @JsonUnwrapped
    @JsonProperty("@odata.id")
    private ODataId oDataId;

    private String id;
    private String name;
    private String description;
    private Oem oem = new Oem();
    private Collection<EventJson> events;

    public EventArrayJson() {
    }

    @Override
    public String getOdataType() {
        return oDataType;
    }

    @Override
    public URI getOdataContext() {
        return oDataContext;
    }

    @Override
    public String getOdataId() {
        return ofNullable(oDataId.toString()).orElseGet(null);
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public Object getOem() {
        return oem;
    }

    @Override
    public Collection<EventJson> getEvents() {
        return ofNullable(events).orElseGet(Collections::emptyList);
    }

    private static final class Oem {
    }

    public static final class EventJson implements Event {
        private EventType eventType;
        private String eventId;
        private String eventTimestamp;
        private String severity;
        private String message;
        private String messageId;
        private Collection<String> messageArgs = new ArrayList<>();
        private String context;
        private ODataId originOfCondition;

        public EventJson() {
        }

        @Override
        public EventType getEventType() {
            return eventType;
        }

        @Override
        public String getEventId() {
            return eventId;
        }

        @Override
        public String getEventTimestamp() {
            return eventTimestamp;
        }

        @Override
        public String getSeverity() {
            return severity;
        }

        @Override
        public String getMessage() {
            return message;
        }

        @Override
        public String getMessageId() {
            return messageId;
        }

        @Override
        public Collection<String> getMessageArgs() {
            return messageArgs;
        }

        @Override
        public String getContext() {
            return context;
        }

        @Override
        public URI getOriginOfCondition() {
            return ofNullable(originOfCondition.toUri()).orElse(null);
        }
    }
}
