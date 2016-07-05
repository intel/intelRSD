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

import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

public final class EthernetSwitchPortVlanDto {
    private final Id id;
    private final String name;
    private final String description;
    private final Status status;
    private final Boolean vlanEnable;
    private final Boolean tagged;
    private final Integer vlanId;

    private EthernetSwitchPortVlanDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        status = builder.status;
        vlanEnable = builder.vlanEnable;
        tagged = builder.tagged;
        vlanId = builder.vlanId;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public Status getStatus() {
        return status;
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public Boolean getTagged() {
        return tagged;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
        private Status status;
        private Boolean vlanEnable;
        private Boolean tagged;
        private Integer vlanId;

        private Builder() {
        }

        public Builder id(Id val) {
            id = val;
            return this;
        }

        public Builder name(String val) {
            name = val;
            return this;
        }

        public Builder description(String val) {
            description = val;
            return this;
        }

        public Builder status(Status val) {
            status = val;
            return this;
        }

        public Builder vlanEnable(Boolean val) {
            vlanEnable = val;
            return this;
        }

        public Builder tagged(Boolean val) {
            tagged = val;
            return this;
        }

        public Builder vlanId(Integer val) {
            vlanId = val;
            return this;
        }

        public EthernetSwitchPortVlanDto build() {
            return new EthernetSwitchPortVlanDto(this);
        }
    }
}
