/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.business.dto.redfish.attributes;

import com.intel.podm.common.types.ServiceKind;

import java.net.URI;

public final class ODataServiceDto {
    private final String name;
    private final ServiceKind kind;
    private final URI url;

    private ODataServiceDto(Builder builder) {
        this.name = builder.name;
        this.kind = builder.kind;
        this.url = builder.url;
    }

    public String getName() {
        return name;
    }

    public ServiceKind getKind() {
        return kind;
    }

    public URI getUrl() {
        return url;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String name;
        private ServiceKind kind;
        private URI url;

        private Builder() {
        }

        public ODataServiceDto build() {
            return new ODataServiceDto(this);
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder kind(ServiceKind kind) {
            this.kind = kind;
            return this;
        }

        public Builder url(URI url) {
            this.url = url;
            return this;
        }
    }
}
