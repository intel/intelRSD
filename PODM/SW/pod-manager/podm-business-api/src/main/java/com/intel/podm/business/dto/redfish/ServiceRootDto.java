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

import java.util.UUID;

public final class ServiceRootDto {
    private final UUID uuid;
    private final String name;
    private final String redfishVersion;
    private final String id;

    private ServiceRootDto(Builder builder) {
        id = builder.id;
        uuid = builder.uuid;
        redfishVersion = builder.serviceVersion;
        name = builder.name;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public UUID getUuid() {
        return uuid;
    }

    public String getRedfishVersion() {
        return redfishVersion;
    }

    public String getName() {
        return name;
    }

    public String getId() {
        return id;
    }

    public static final class Builder {
        private UUID uuid;
        private String name;
        private String serviceVersion;
        private String id;

        private Builder() {
        }

        public Builder uuid(UUID uuid) {
            this.uuid = uuid;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder redfishVersion(String serviceVersion) {
            this.serviceVersion = serviceVersion;
            return this;
        }

        public ServiceRootDto build() {
            return new ServiceRootDto(this);
        }

        public Builder id(String id) {
            this.id = id;
            return this;
        }
    }
}
