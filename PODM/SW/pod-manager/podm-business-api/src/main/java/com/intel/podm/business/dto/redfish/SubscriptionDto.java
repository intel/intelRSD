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

import java.util.List;

public final class SubscriptionDto {
    private final String id;
    private final String name;
    private final String destination;
    private final String context;
    private final List<String> eventTypes;
    private final String protocol;

    private SubscriptionDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        destination = builder.destination;
        context = builder.context;
        eventTypes = builder.eventTypes;
        protocol = builder.protocol;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDestination() {
        return destination;
    }

    public String getContext() {
        return context;
    }

    public List<String> getEventTypes() {
        return eventTypes;
    }

    public String getProtocol() {
        return protocol;
    }

    public static final class Builder {
        private String id;
        private String name;
        private String destination;
        private String context;
        private List<String> eventTypes;
        private String protocol;

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

        public Builder destination(String destination) {
            this.destination = destination;
            return this;
        }

        public Builder context(String context) {
            this.context = context;
            return this;
        }

        public Builder eventTypes(List<String> eventTypes) {
            this.eventTypes = eventTypes;
            return this;
        }

        public Builder protocol(String protocol) {
            this.protocol = protocol;
            return this;
        }

        public SubscriptionDto build() {
            return new SubscriptionDto(this);
        }
    }
}
