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

import com.intel.podm.common.types.events.EventType;

public final class EventDto {
    private EventType eventType;
    private String originOfCondition;

    private EventDto(Builder builder) {
        eventType = builder.eventType;
        originOfCondition = builder.originOfCondition;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public EventType getEventType() {
        return eventType;
    }

    public String getOriginOfCondition() {
        return originOfCondition;
    }

    @Override
    public String toString() {
        return "EventDto{eventType=" + eventType + ", originOfCondition='" + originOfCondition + "'}";
    }

    public static class Builder {
        private EventType eventType;
        private String originOfCondition;

        public Builder eventType(EventType eventType) {
            this.eventType = eventType;
            return this;
        }

        public Builder originOfCondition(String originOfCondition) {
            this.originOfCondition = originOfCondition;
            return this;
        }

        public EventDto build() {
            return new EventDto(this);
        }
    }
}
