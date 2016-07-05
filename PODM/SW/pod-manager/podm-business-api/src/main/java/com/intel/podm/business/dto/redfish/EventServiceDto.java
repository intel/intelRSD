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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.common.types.Status;

import java.util.Collection;

public final class EventServiceDto {
    private final String id;
    private final Status status;
    private final Boolean serviceEnabled;
    private final Collection<String> eventTypesForSubscription;

    private final String name;

    private EventServiceDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        status = builder.status;
        serviceEnabled = builder.serviceEnabled;
        eventTypesForSubscription = builder.eventTypesForSubscription;
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

    public Collection<String> getEventTypesForSubscription() {
        return eventTypesForSubscription;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String id;
        private String name;
        private Status status;
        private Boolean serviceEnabled;
        private Collection<String> eventTypesForSubscription;

        private Builder() {
        }

        public Builder id(String val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder serviceEnabled(Boolean val) {
            serviceEnabled = val;
            return this;
        }

        public Builder eventTypesForSubscription(Collection<String> val) {
            eventTypesForSubscription = val;
            return this;
        }

        public EventServiceDto build() {
            return new EventServiceDto(this);
        }
    }
}
