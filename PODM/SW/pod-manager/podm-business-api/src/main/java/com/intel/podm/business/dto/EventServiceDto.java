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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.common.types.redfish.IgnoreAutomaticOem;

import java.util.Collection;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name", "description", "serviceEnabled", "status",
    "deliveryRetryAttempts", "deliveryRetryIntervalSeconds", "eventTypesForSubscription", "subscriptions",
    "links", "actions", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class EventServiceDto extends RedfishDto {
    private Boolean serviceEnabled;
    private Long deliveryRetryAttempts;
    private Long deliveryRetryIntervalSeconds;
    private Collection<EventType> eventTypesForSubscription;
    private Status status;
    @IgnoreAutomaticOem
    private final Object actions = new Object();
    private ODataId subscriptions;

    public EventServiceDto() {
        super("#EventService.v1_0_2.EventService");
    }

    public Boolean getServiceEnabled() {
        return serviceEnabled;
    }

    public void setServiceEnabled(Boolean serviceEnabled) {
        this.serviceEnabled = serviceEnabled;
    }

    public Long getDeliveryRetryAttempts() {
        return deliveryRetryAttempts;
    }

    public void setDeliveryRetryAttempts(Long deliveryRetryAttempts) {
        this.deliveryRetryAttempts = deliveryRetryAttempts;
    }

    public Long getDeliveryRetryIntervalSeconds() {
        return deliveryRetryIntervalSeconds;
    }

    public void setDeliveryRetryIntervalSeconds(Long deliveryRetryIntervalSeconds) {
        this.deliveryRetryIntervalSeconds = deliveryRetryIntervalSeconds;
    }

    public Collection<EventType> getEventTypesForSubscription() {
        return eventTypesForSubscription;
    }

    public void setEventTypesForSubscription(Collection<EventType> eventTypesForSubscription) {
        this.eventTypesForSubscription = eventTypesForSubscription;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public Object getActions() {
        return actions;
    }

    public ODataId getSubscriptions() {
        return subscriptions;
    }

    public void setSubscriptions(ODataId subscriptions) {
        this.subscriptions = subscriptions;
    }
}
