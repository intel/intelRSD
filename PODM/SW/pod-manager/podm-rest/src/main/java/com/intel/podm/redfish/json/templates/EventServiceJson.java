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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.events.EventType;

import java.util.ArrayList;
import java.util.Collection;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "serviceEnabled", "status",
    "deliveryRetryAttempts", "deliveryRetryIntervalSeconds", "eventTypesForSubscription", "subscriptions",
    "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class EventServiceJson extends BaseJson {
    public String id;
    public String name;
    public Status status;
    public Boolean serviceEnabled;
    public ODataId subscriptions;
    public String description;
    public Long deliveryRetryAttempts;
    public Long deliveryRetryIntervalSeconds;
    public Object oem = new Object();
    public Object actions = new Object();
    public Collection<EventType> eventTypesForSubscription = new ArrayList<>();

    public EventServiceJson() {
        super("#EventService.v1_0_0.EventService");
    }
}
