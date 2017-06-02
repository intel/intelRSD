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
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

public final class FabricDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Protocol fabricType;
    private final Status status;
    private final Integer maxZones;
    private final Links links;

    private FabricDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        fabricType = builder.fabricType;
        maxZones = builder.maxZones;
        links = new Links();
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

    public Protocol getFabricType() {
        return fabricType;
    }

    public Status getStatus() {
        return status;
    }

    public Integer getMaxZones() {
        return maxZones;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Links implements RedfishResource.Links {
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private Protocol fabricType;
        private Integer maxZones;

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

        public Builder unknownOems(List<UnknownOemDto> unknownOems) {
            this.unknownOems = unknownOems;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder maxZones(Integer maxZones) {
            this.maxZones = maxZones;
            return this;
        }

        public Builder fabricType(Protocol fabricType) {
            this.fabricType = fabricType;
            return this;
        }

        public FabricDto build() {
            return new FabricDto(this);
        }
    }
}
