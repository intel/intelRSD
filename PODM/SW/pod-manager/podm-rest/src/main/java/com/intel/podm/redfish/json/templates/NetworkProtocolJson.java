/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.redfish.json.templates;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_NULL;

@JsonPropertyOrder({
        "@odata.context", "@odata.id", "@odata.type",  "id", "name",
        "description", "status", "hostName", "fqdn", "http", "https",
        "ipmi", "ssh", "snmp", "virtualMedia", "ssdp", "telnet", "kvmip", "oem"
})
public class NetworkProtocolJson extends BaseJson {
    public Id id;
    public String name;
    public String description;
    public Status status;
    public String hostName;
    @JsonProperty("FQDN")
    public String fqdn;

    @JsonProperty("HTTP")
    public ProtocolJson http;
    @JsonProperty("HTTPS")
    public ProtocolJson https;
    @JsonProperty("IPMI")
    public ProtocolJson ipmi;
    @JsonProperty("SSH")
    public ProtocolJson ssh;
    @JsonProperty("SNMP")
    public ProtocolJson snmp;
    @JsonProperty("VirtualMedia")
    public ProtocolJson virtualMedia;
    @JsonProperty("SSDP")
    public SimpleServiceDiscoveryProtocolJson ssdp;
    @JsonProperty("Telnet")
    public ProtocolJson telnet;
    @JsonProperty("KVMIP")
    public ProtocolJson kvmip;

    public final Object oem = new Object();

    public NetworkProtocolJson() {
        super("#ManagerNetworkProtocol.1.0.0.ManagerNetworkProtocol");
    }

    @JsonPropertyOrder({"protocolEnabled", "port"})
    public static class ProtocolJson {
        public Boolean protocolEnabled;

        @JsonInclude(NON_NULL)
        public Integer port;
    }

    @JsonPropertyOrder({"protocolEnabled", "port", "notifyMulticastIntervalSeconds", "notifyTtl", "notifyIpV6Scope"})
    public static final class SimpleServiceDiscoveryProtocolJson extends ProtocolJson {
        @JsonInclude(NON_NULL)
        public Integer notifyMulticastIntervalSeconds;

        @JsonInclude(NON_NULL)
        @JsonProperty("NotifyTTL")
        public Integer notifyTtl;

        @JsonInclude(NON_NULL)
        @JsonProperty("NotifyIPv6Scope")
        public NotifyIpV6Scope notifyIpV6Scope;
    }
}
