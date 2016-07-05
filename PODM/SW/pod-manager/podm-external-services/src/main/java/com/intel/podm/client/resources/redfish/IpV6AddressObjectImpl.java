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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonIgnoreProperties;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.api.resources.redfish.IpV6AddressObject;
import com.intel.podm.common.types.AddressState;
import com.intel.podm.common.types.IpV6AddressOrigin;

@JsonIgnoreProperties(ignoreUnknown = true)
public class IpV6AddressObjectImpl implements IpV6AddressObject {
    @JsonProperty("Address")
    private String address;
    @JsonProperty("PrefixLength")
    private Integer prefixLength;
    @JsonProperty("AddressOrigin")
    private IpV6AddressOrigin addressOrigin;
    @JsonProperty("AddressState")
    private AddressState addressState;

    @Override
    public String getAddress() {
        return address;
    }

    @Override
    public Integer getPrefixLength() {
        return prefixLength;
    }

    @Override
    public IpV6AddressOrigin getAddressOrigin() {
        return addressOrigin;
    }

    @Override
    public AddressState getAddressState() {
        return addressState;
    }
}
