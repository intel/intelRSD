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

package com.intel.podm.business.dto.redfish.attributes;


public final class IpV6AddressPolicyDto {
    private final String prefix;
    private final Integer precedence;
    private final Integer label;

    private IpV6AddressPolicyDto(Builder builder) {
        prefix = builder.prefix;
        precedence = builder.precedence;
        label = builder.label;
    }

    public String getPrefix() {
        return prefix;
    }

    public Integer getPrecedence() {
        return precedence;
    }

    public Integer getLabel() {
        return label;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public static final class Builder {
        private String prefix;
        private Integer precedence;
        private Integer label;

        private Builder() {
        }

        public Builder prefix(String prefix) {
            this.prefix = prefix;
            return this;
        }

        public Builder precedence(Integer precedence) {
            this.precedence = precedence;
            return this;
        }

        public Builder label(Integer label) {
            this.label = label;
            return this;
        }

        public IpV6AddressPolicyDto build() {
            return new IpV6AddressPolicyDto(this);
        }
    }
}
