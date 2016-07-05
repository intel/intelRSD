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

package com.intel.podm.eventing.dto;

import java.util.Collection;
import java.util.UUID;

public final class EventArrayDto {
    private final String id;
    private final String name;
    private final String description;
    private final UUID serviceUuid;
    private final Collection<EventDto> events;

    private EventArrayDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        serviceUuid = builder.serviceUuid;
        events = builder.events;
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

    public String getDescription() {
        return description;
    }

    public UUID getServiceUuid() {
        return serviceUuid;
    }

    public Collection<EventDto> getEvents() {
        return events;
    }

    @Override
    public String toString() {
        return "EventArrayDto{serviceUuid=" + serviceUuid + ", events=" + events + '}';
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private Collection<EventDto> events;
        private UUID serviceUuid;

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

        public Builder serviceUuid(UUID serviceUuid) {
            this.serviceUuid = serviceUuid;
            return this;
        }

        public Builder events(Collection<EventDto> events) {
            this.events = events;
            return this;
        }

        public EventArrayDto build() {
            return new EventArrayDto(this);
        }
    }
}
