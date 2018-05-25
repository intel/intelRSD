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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonTypeInfo;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.Protocol;

import static com.fasterxml.jackson.annotation.JsonTypeInfo.Id.CLASS;

@JsonTypeInfo(
    use = CLASS,
    defaultImpl = IpTransportDetailsResource.class
)
@OdataTypes({
    "Endpoint\\.IPTransportDetails"
})
public class IpTransportDetailsResource extends ExternalServiceResourceImpl implements ExternalServiceResource {

    @JsonProperty("IPv4Address")
    private IpV4AddressObject ipv4Address;

    @JsonProperty("IPv6Address")
    private IpV6AddressObject ipv6Address;

    @JsonProperty("Port")
    private Integer port;

    @JsonProperty("TransportProtocol")
    private Protocol transportProtocol;

    public Protocol getTransportProtocol() {
        return transportProtocol;
    }

    public IpV4AddressObject getIpv4Address() {
        return ipv4Address;
    }

    public IpV6AddressObject getIpv6Address() {
        return ipv6Address;
    }

    public Integer getPort() {
        return port;
    }
}
