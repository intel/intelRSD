/*
 * Copyright (c) 2016-2017 Intel Corporation
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
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;
import java.util.Set;

public final class ZoneDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final Links links;

    private ZoneDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        links = new Links(builder.endpoints, builder.involvedSwitches);
    }

    public static Builder newBuilder() {
        return new Builder();
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

    @Override
    public List<UnknownOemDto> getUnknownOems() {
        return unknownOems;
    }

    @Override
    public Links getLinks() {
        return links;
    }

    public Status getStatus() {
        return status;
    }

    public static final class Links implements RedfishResource.Links {
        private final Set<Context> endpoints;
        private final Set<Context> involvedSwitches;

        public Links(Set<Context> endpoints, Set<Context> involvedSwitches) {
            this.endpoints = endpoints;
            this.involvedSwitches = involvedSwitches;
        }

        public Set<Context> getEndpoints() {
            return endpoints;
        }

        public Set<Context> getInvolvedSwitches() {
            return involvedSwitches;
        }
    }

    public static class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Set<Context> endpoints;
        private Set<Context> involvedSwitches;
        private Status status;

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

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder endpoints(Set<Context> endpoints) {
            this.endpoints = endpoints;
            return this;
        }

        public Builder involvedSwitches(Set<Context> involvedSwitches) {
            this.involvedSwitches = involvedSwitches;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public ZoneDto build() {
            return new ZoneDto(this);
        }
    }
}
