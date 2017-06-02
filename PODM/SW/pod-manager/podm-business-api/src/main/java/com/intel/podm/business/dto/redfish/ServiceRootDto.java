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

import java.util.List;
import java.util.UUID;

public final class ServiceRootDto extends BaseDto {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final String redfishVersion;
    private final UUID uuid;

    private ServiceRootDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        redfishVersion = builder.redfishVersion;
        uuid = builder.uuid;
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

    public String getRedfishVersion() {
        return redfishVersion;
    }

    public UUID getUuid() {
        return uuid;
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private UUID uuid;
        private String redfishVersion;

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

        public Builder redfishVersion(String redfishVersion) {
            this.redfishVersion = redfishVersion;
            return this;
        }

        public Builder uuid(UUID uuid) {
            this.uuid = uuid;
            return this;
        }

        public ServiceRootDto build() {
            return new ServiceRootDto(this);
        }
    }
}
