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
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.redfish.RedfishResource;

import java.util.List;

public final class RemoteTargetDto extends BaseDto implements RedfishResource {
    private final String id;
    private final String name;
    private final String description;
    private final List<UnknownOemDto> unknownOems;
    private final Status status;
    private final String type;
    private final List<RemoteTargetIscsiAddressDto> iscsiAddresses;
    private final String iscsiInitiatorIqn;

    private RemoteTargetDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        unknownOems = builder.unknownOems;
        status = builder.status;
        type = builder.type;
        iscsiAddresses = builder.iscsiAddresses;
        iscsiInitiatorIqn = builder.iscsiInitiatorIqn;
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

    public Status getStatus() {
        return status;
    }

    public String getType() {
        return type;
    }

    public List<RemoteTargetIscsiAddressDto> getIscsiAddresses() {
        return iscsiAddresses;
    }

    public String getIscsiInitiatorIqn() {
        return iscsiInitiatorIqn;
    }

    @Override
    public Links getLinks() {
        throw new UnsupportedOperationException("Links are not available in this resource");
    }

    public static final class Builder {
        private String id;
        private String name;
        private String description;
        private List<UnknownOemDto> unknownOems;
        private Status status;
        private String type;
        private List<RemoteTargetIscsiAddressDto> iscsiAddresses;
        private String iscsiInitiatorIqn;

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

        public Builder type(String type) {
            this.type = type;
            return this;
        }

        public Builder iscsiAddresses(List<RemoteTargetIscsiAddressDto> iscsiAddresses) {
            this.iscsiAddresses = iscsiAddresses;
            return this;
        }

        public Builder iscsiInitiatorIqn(String iscsiInitiatorIqn) {
            this.iscsiInitiatorIqn = iscsiInitiatorIqn;
            return this;
        }

        public RemoteTargetDto build() {
            return new RemoteTargetDto(this);
        }
    }
}
