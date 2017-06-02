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


import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.IpV6AddressOrigin;

public final class IpV6AddressDto {
    private final String address;
    private final Integer prefixLength;
    private final IpV6AddressOrigin addressOrigin;
    private final AddressState addressState;
    private final UnknownOemDto oem;

    private IpV6AddressDto(Builder builder) {
        address = builder.address;
        prefixLength = builder.prefixLength;
        addressOrigin = builder.addressOrigin;
        addressState = builder.addressState;
        oem = builder.oem;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getAddress() {
        return address;
    }

    public Integer getPrefixLength() {
        return prefixLength;
    }

    public IpV6AddressOrigin getAddressOrigin() {
        return addressOrigin;
    }

    public AddressState getAddressState() {
        return addressState;
    }

    public UnknownOemDto getOem() {
        return oem;
    }

    public static final class Builder {
        private String address;
        private Integer prefixLength;
        private IpV6AddressOrigin addressOrigin;
        private AddressState addressState;
        private UnknownOemDto oem;

        private Builder() {
        }

        public Builder address(String address) {
            this.address = address;
            return this;
        }

        public Builder prefixLength(Integer prefixLength) {
            this.prefixLength = prefixLength;
            return this;
        }

        public Builder addressOrigin(IpV6AddressOrigin addressOrigin) {
            this.addressOrigin = addressOrigin;
            return this;
        }

        public Builder addressState(AddressState addressState) {
            this.addressState = addressState;
            return this;
        }

        public Builder oem(UnknownOemDto oem) {
            this.oem = oem;
            return this;
        }

        public IpV6AddressDto build() {
            return new IpV6AddressDto(this);
        }
    }
}
