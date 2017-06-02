/*
 * Copyright (c) 2016-2017 Intel Corporation
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

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type",  "id", "name",
    "description", "status", "hostName", "fqdn", "http", "https",
    "ipmi", "ssh", "snmp", "virtualMedia", "ssdp", "telnet", "kvmip", "oem"
})
@SuppressWarnings({"checkstyle:VisibilityModifier"})
public class NetworkProtocolJson extends BaseResourceJson {
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

    public NetworkProtocolJson() {
        super("#ManagerNetworkProtocol.v1_0_0.ManagerNetworkProtocol");
    }

    @JsonPropertyOrder({"protocolEnabled", "port"})
    public static class ProtocolJson {
        public Boolean protocolEnabled;
        public Integer port;
    }

    @JsonPropertyOrder({"protocolEnabled", "port", "notifyMulticastIntervalSeconds", "notifyTtl", "notifyIpV6Scope"})
    public static final class SimpleServiceDiscoveryProtocolJson extends ProtocolJson {
        public Integer notifyMulticastIntervalSeconds;

        @JsonProperty("NotifyTTL")
        public Integer notifyTtl;

        @JsonProperty("NotifyIPv6Scope")
        public NotifyIpV6Scope notifyIpV6Scope;
    }
}
