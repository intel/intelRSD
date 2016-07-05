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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Status;
import com.intel.podm.rest.odataid.ODataId;

import java.util.Collection;

import static com.google.common.collect.Lists.newArrayList;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id", "name", "status", "serviceEnabled",
        "eventTypesForSubscription", "subscriptions",
        "actions", "oem"
})
public class EventServiceJson extends BaseJson {
    public String id;
    public String name;
    public Status status;
    public Boolean serviceEnabled;
    public Collection<String> eventTypesForSubscription;
    public ODataId subscriptions;
    public final Actions actions = new Actions();
    public Object oem = new Object();

    public EventServiceJson() {
        super("#EventService.1.0.0.EventService");
    }

    @JsonPropertyOrder({"#EventService.SendTestEvent", "oem"})
    public static final class Actions {
        @JsonProperty("#EventService.SendTestEvent")
        public final SendTestEvent sendTestEvent;
        public final Object oem;

        public Actions() {
            this.sendTestEvent = new SendTestEvent();
            this.oem = new Object();
        }

        @JsonPropertyOrder({"target", "EventType@Redfish.AllowableValues"})
        public static final class SendTestEvent {
            @JsonProperty("target")
            public final String target;
            @JsonProperty("EventType@Redfish.AllowableValues")
            public final Collection<String> allowableValues;

            public SendTestEvent() {
                this.target = "/redfish/v1/EventService/Actions/EventService.SendTestEvent";
                this.allowableValues = newArrayList("StatusChange", "ResourceUpdated", "ResourceAdded", "ResourceRemoved", "Alert");
            }
        }
    }
}
