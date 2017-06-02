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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.UnknownOemDto;
import com.intel.podm.business.services.redfish.odataid.ODataId;
import com.intel.podm.common.types.events.EventType;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

public class EventSubscriptionDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<EventType> eventTypes = new ArrayList<>();
    private final String subscriptionContext;
    private final String protocol;
    private final String destination;
    private final List<ODataId> originResources = new ArrayList<>();

    public EventSubscriptionDto(Builder builder) {
        this.id = builder.id;
        this.name = builder.name;
        this.description = builder.description;
        this.eventTypes.addAll(builder.eventTypes);
        this.subscriptionContext = builder.subscriptionContext;
        this.protocol = builder.protocol;
        this.destination = builder.destination;
        this.originResources.addAll(builder.originResources);
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

    public List<EventType> getEventTypes() {
        return eventTypes;
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

    public List<ODataId> getOriginResources() {
        return originResources;
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
        private Collection<EventType> eventTypes = new ArrayList<>();
        private String subscriptionContext;
        private String protocol;
        private String destination;
        private List<ODataId> originResources = new ArrayList<>();

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


        public Builder eventTypesForSubscription(Collection<EventType> eventTypesForSubscription) {
            this.eventTypes = eventTypesForSubscription;
            return this;
        }

        public Builder originResources(List<ODataId> originResources) {
            this.originResources = originResources;
            return this;
        }

        public Builder subscriptionContext(String subscriptionContext) {
            this.subscriptionContext = subscriptionContext;
            return this;
        }

        public Builder protocol(String protocol) {
            this.protocol = protocol;
            return this;
        }

        public Builder destination(String destination) {
            this.destination = destination;
            return this;
        }

        public EventSubscriptionDto build() {
            return new EventSubscriptionDto(this);
        }
    }
}
