/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.dto;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.nodecomposer.types.AddressState;
import com.intel.rsd.nodecomposer.types.IpV4AddressOrigin;
import com.intel.rsd.nodecomposer.types.IpV6AddressOrigin;
import lombok.Getter;
import lombok.Setter;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

public final class IpAddresses {
    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"address", "subnetMask", "addressOrigin", "gateway", "oem"})
    public static final class IpV4AddressDto {
        private String address;
        private String subnetMask;
        private IpV4AddressOrigin addressOrigin;
        private String gateway;
        private JsonNode oem;
    }

    @Getter
    @Setter
    @JsonInclude(NON_NULL)
    @JsonPropertyOrder({"address", "prefixLength", "addressOrigin", "addressState", "oem"})
    public static final class IpV6AddressDto {
        private String address;
        private Integer prefixLength;
        private IpV6AddressOrigin addressOrigin;
        private AddressState addressState;
        private JsonNode oem;
    }
}
