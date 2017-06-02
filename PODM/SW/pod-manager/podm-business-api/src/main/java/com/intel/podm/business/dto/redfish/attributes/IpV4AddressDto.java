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

import com.intel.podm.common.types.IpV4AddressOrigin;

public final class IpV4AddressDto {
    private final String address;
    private final String subnetMask;
    private final IpV4AddressOrigin addressOrigin;
    private final String gateway;
    private final UnknownOemDto oem;

    private IpV4AddressDto(Builder builder) {
        address = builder.address;
        subnetMask = builder.subnetMask;
        addressOrigin = builder.addressOrigin;
        gateway = builder.gateway;
        oem = builder.oem;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public String getAddress() {
        return address;
    }

    public String getSubnetMask() {
        return subnetMask;
    }

    public IpV4AddressOrigin getAddressOrigin() {
        return addressOrigin;
    }

    public String getGateway() {
        return gateway;
    }

    public UnknownOemDto getOem() {
        return oem;
    }

    public static final class Builder {
        private String address;
        private String subnetMask;
        private IpV4AddressOrigin addressOrigin;
        private String gateway;
        private UnknownOemDto oem;

        private Builder() {
        }

        public Builder address(String address) {
            this.address = address;
            return this;
        }

        public Builder subnetMask(String subnetMask) {
            this.subnetMask = subnetMask;
            return this;
        }

        public Builder addressOrigin(IpV4AddressOrigin addressOrigin) {
            this.addressOrigin = addressOrigin;
            return this;
        }

        public Builder gateway(String gateway) {
            this.gateway = gateway;
            return this;
        }

        public Builder oem(UnknownOemDto oem) {
            this.oem = oem;
            return this;
        }

        public IpV4AddressDto build() {
            return new IpV4AddressDto(this);
        }
    }
}
