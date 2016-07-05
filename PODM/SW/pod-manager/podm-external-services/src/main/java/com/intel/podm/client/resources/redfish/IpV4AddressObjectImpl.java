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
import com.intel.podm.client.api.resources.redfish.IpV4AddressObject;
import com.intel.podm.common.types.IpV4AddressOrigin;

@JsonIgnoreProperties(ignoreUnknown = true)
public class IpV4AddressObjectImpl implements IpV4AddressObject {
    @JsonProperty("Address")
    private String address;
    @JsonProperty("SubnetMask")
    private String subnetMask;
    @JsonProperty("AddressOrigin")
    private IpV4AddressOrigin addressOrigin;
    @JsonProperty("Gateway")
    private String gateway;

    @Override
    public String getAddress() {
        return address;
    }

    @Override
    public String getSubnetMask() {
        return subnetMask;
    }

    @Override
    public IpV4AddressOrigin getAddressOrigin() {
        return addressOrigin;
    }

    @Override
    public String getGateway() {
        return gateway;
    }
}
