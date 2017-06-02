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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.Collection;
import java.util.List;

public final class EventServiceDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final Boolean serviceEnabled;
    private final Long deliveryRetryAttempts;
    private final Long deliveryRetryIntervalSeconds;
    private final Collection<EventType> eventTypesForSubscription;
    private final Status status;
    private final ODataId subscriptions;

    public EventServiceDto(Builder builder) {
        this.id = builder.id;
        this.name = builder.name;
        this.status = builder.status;
        this.description = builder.description;
        this.subscriptions = builder.subscriptions;
        this.serviceEnabled = builder.serviceEnabled;
        this.deliveryRetryAttempts = builder.deliveryRetryAttempts;
        this.eventTypesForSubscription = builder.eventTypesForSubscription;
        this.deliveryRetryIntervalSeconds = builder.deliveryRetryIntervalSeconds;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public Status getStatus() {
        return status;
    }

    public Boolean getServiceEnabled() {
        return serviceEnabled;
    }

    public Collection<EventType> getEventTypesForSubscription() {
        return eventTypesForSubscription;
    }

    @Override
    public String getDescription() {
        return description;
    }

    public Long getDeliveryRetryAttempts() {
        return deliveryRetryAttempts;
    }

    public Long getDeliveryRetryIntervalSeconds() {
        return deliveryRetryIntervalSeconds;
    }

    public ODataId getSubscriptions() {
        return subscriptions;
    }

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return null;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private Status status;
        private Boolean serviceEnabled;
        private Collection<EventType> eventTypesForSubscription;
        private Long deliveryRetryAttempts;
        private Long deliveryRetryIntervalSeconds;
        private ODataId subscriptions;

        private Builder() {
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder serviceEnabled(Boolean serviceEnabled) {
            this.serviceEnabled = serviceEnabled;
            return this;
        }

        public Builder eventTypesForSubscription(Collection<EventType> eventTypesForSubscription) {
            this.eventTypesForSubscription = eventTypesForSubscription;
            return this;
        }

        public Builder deliveryRetryAttempts(Long deliveryRetryAttempts) {
            this.deliveryRetryAttempts = deliveryRetryAttempts;
            return this;
        }

        public Builder deliveryRetryIntervalSeconds(Long deliveryRetryIntervalSeconds) {
            this.deliveryRetryIntervalSeconds = deliveryRetryIntervalSeconds;
            return this;
        }

        public Builder subscriptions(ODataId subscriptions) {
            this.subscriptions = subscriptions;
            return this;
        }

        public EventServiceDto build() {
            return new EventServiceDto(this);
        }
    }
}
