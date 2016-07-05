/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.rest.representation.json.templates;


import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonUnwrapped;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.events.EventType;

import java.net.URI;
import java.util.ArrayList;
import java.util.Collection;

public class EventArrayJson {
    @JsonProperty("@odata.type")
    public String oDataType;

    @JsonProperty("@odata.context")
    public URI oDataContext;

    @JsonUnwrapped
    @JsonProperty("@odata.id")
    public ODataId oDataId;

    public String id;
    public String name;
    public String description;
    public Oem oem = new Oem();
    public final Collection<EventJson> events = new ArrayList<>();

    private static final class Oem {
    }

    public static final class EventJson {
        public EventType eventType;
        public String eventId;
        public String eventTimestamp;
        public String severity;
        public String message;
        public String messageId;
        public Collection<String> messageArgs = new ArrayList<>();
        public String context;
        public ODataId originOfCondition;
    }
}
