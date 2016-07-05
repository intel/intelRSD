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

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.events.EventType;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type", "id",
        "name", "destination", "eventTypes", "context", "protocol"
})
public class SubscriptionJson extends BaseJson {
    public String id;
    public String name;
    public String destination;
    public List<EventType> eventTypes = new ArrayList<>();
    public String context;
    public String protocol;

    public SubscriptionJson() {
        super("#EventDestination.1.0.0.EventDestination");
    }
}
