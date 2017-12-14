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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.events.EventType;

import java.util.ArrayList;
import java.util.List;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description",
    "destination", "eventTypes", "subscriptionContext", "protocol", "originResources"
})
public class EventSubscriptionDto extends RedfishDto {
    private List<EventType> eventTypes = new ArrayList<>();
    @JsonProperty("Context")
    private String subscriptionContext;
    private String protocol;
    private String destination;
    private List<ODataId> originResources = new ArrayList<>();

    public EventSubscriptionDto() {
        super("#EventDestination.v1_1_0.EventDestination");
    }

    public List<EventType> getEventTypes() {
        return eventTypes;
    }

    public void setEventTypes(List<EventType> eventTypes) {
        this.eventTypes = eventTypes;
    }

    public void setSubscriptionContext(String subscriptionContext) {
        this.subscriptionContext = subscriptionContext;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public void setOriginResources(List<ODataId> originResources) {
        this.originResources = originResources;
    }

    public String getSubscriptionContext() {
        return subscriptionContext;
    }

    public String getProtocol() {
        return protocol;
    }

    public String getDestination() {
        return destination;
    }

    public void setDestination(String destination) {
        this.destination = destination;
    }

    public List<ODataId> getOriginResources() {
        return originResources;
    }

}
