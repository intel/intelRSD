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

package com.intel.podm.business.dto;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.fasterxml.jackson.annotation.JsonPropertyOrder;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

@JsonPropertyOrder({
    "@odata.context", "@odata.id", "@odata.type", "id", "name",
    "description", "status", "hostName", "fqdn", "http", "https",
    "ipmi", "ssh", "snmp", "virtualMedia", "ssdp", "telnet", "kvmip", "oem"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public final class NetworkProtocolDto extends RedfishDto {
    private Status status;
    private String hostName;
    @JsonProperty("FQDN")
    private String fqdn;
    @JsonProperty("HTTP")
    private ProtocolDto http;
    @JsonProperty("HTTPS")
    private ProtocolDto https;
    @JsonProperty("IPMI")
    private ProtocolDto ipmi;
    @JsonProperty("SSH")
    private ProtocolDto ssh;
    @JsonProperty("SNMP")
    private ProtocolDto snmp;
    @JsonProperty("VirtualMedia")
    private ProtocolDto virtualMedia;
    @JsonProperty("SSDP")
    private SimpleServiceDiscoveryProtocolDto ssdp;
    @JsonProperty("Telnet")
    private ProtocolDto telnet;
    @JsonProperty("KVMIP")
    private ProtocolDto kvmip;

    public NetworkProtocolDto() {
        super("#ManagerNetworkProtocol.v1_0_0.ManagerNetworkProtocol");
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public String getHostName() {
        return hostName;
    }

    public void setHostName(String hostName) {
        this.hostName = hostName;
    }

    public String getFqdn() {
        return fqdn;
    }

    public void setFqdn(String fqdn) {
        this.fqdn = fqdn;
    }

    public ProtocolDto getHttp() {
        return http;
    }

    public void setHttp(ProtocolDto http) {
        this.http = http;
    }

    public ProtocolDto getHttps() {
        return https;
    }

    public void setHttps(ProtocolDto https) {
        this.https = https;
    }

    public ProtocolDto getIpmi() {
        return ipmi;
    }

    public void setIpmi(ProtocolDto ipmi) {
        this.ipmi = ipmi;
    }

    public ProtocolDto getSsh() {
        return ssh;
    }

    public void setSsh(ProtocolDto ssh) {
        this.ssh = ssh;
    }

    public ProtocolDto getSnmp() {
        return snmp;
    }

    public void setSnmp(ProtocolDto snmp) {
        this.snmp = snmp;
    }

    public ProtocolDto getVirtualMedia() {
        return virtualMedia;
    }

    public void setVirtualMedia(ProtocolDto virtualMedia) {
        this.virtualMedia = virtualMedia;
    }

    public SimpleServiceDiscoveryProtocolDto getSsdp() {
        return ssdp;
    }

    public void setSsdp(SimpleServiceDiscoveryProtocolDto ssdp) {
        this.ssdp = ssdp;
    }

    public ProtocolDto getTelnet() {
        return telnet;
    }

    public void setTelnet(ProtocolDto telnet) {
        this.telnet = telnet;
    }

    public ProtocolDto getKvmip() {
        return kvmip;
    }

    public void setKvmip(ProtocolDto kvmip) {
        this.kvmip = kvmip;
    }

    @JsonPropertyOrder({"protocolEnabled", "port"})
    public static class ProtocolDto {
        private Boolean protocolEnabled;
        private Integer port;

        public Boolean getProtocolEnabled() {
            return protocolEnabled;
        }

        public void setProtocolEnabled(Boolean protocolEnabled) {
            this.protocolEnabled = protocolEnabled;
        }

        public Integer getPort() {
            return port;
        }

        public void setPort(Integer port) {
            this.port = port;
        }
    }

    @JsonPropertyOrder({"protocolEnabled", "port", "notifyMulticastIntervalSeconds", "notifyTtl", "notifyIpV6Scope"})
    public static final class SimpleServiceDiscoveryProtocolDto extends ProtocolDto {
        private Integer notifyMulticastIntervalSeconds;
        @JsonProperty("NotifyTTL")
        private Integer notifyTtl;
        @JsonProperty("NotifyIPv6Scope")
        private NotifyIpV6Scope notifyIpV6Scope;

        public Integer getNotifyMulticastIntervalSeconds() {
            return notifyMulticastIntervalSeconds;
        }

        public void setNotifyMulticastIntervalSeconds(Integer notifyMulticastIntervalSeconds) {
            this.notifyMulticastIntervalSeconds = notifyMulticastIntervalSeconds;
        }

        public Integer getNotifyTtl() {
            return notifyTtl;
        }

        public void setNotifyTtl(Integer notifyTtl) {
            this.notifyTtl = notifyTtl;
        }

        public NotifyIpV6Scope getNotifyIpV6Scope() {
            return notifyIpV6Scope;
        }

        public void setNotifyIpV6Scope(NotifyIpV6Scope notifyIpV6Scope) {
            this.notifyIpV6Scope = notifyIpV6Scope;
        }
    }
}
