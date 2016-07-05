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

package com.intel.podm.business.dto.redfish;

import com.intel.podm.business.dto.redfish.attributes.ProtocolDto;
import com.intel.podm.business.dto.redfish.attributes.SimpleServiceDiscoveryProtocolDto;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Status;

public final class NetworkProtocolDto {
    private final Id id;
    private final String name;
    private final String description;
    private final Status status;
    private final String hostName;
    private final String fqdn;

    private final ProtocolDto http;
    private final ProtocolDto https;
    private final ProtocolDto ipmi;
    private final ProtocolDto ssh;
    private final ProtocolDto snmp;
    private final ProtocolDto virtualMedia;
    private final SimpleServiceDiscoveryProtocolDto ssdp;
    private final ProtocolDto telnet;
    private final ProtocolDto kvmip;

    private NetworkProtocolDto(Builder builder) {
        id = builder.id;
        name = builder.name;
        description = builder.description;
        status = builder.status;
        hostName = builder.hostName;
        fqdn = builder.fqdn;
        http = builder.http;
        https = builder.https;
        ipmi = builder.ipmi;
        ssh = builder.ssh;
        snmp = builder.snmp;
        virtualMedia = builder.virtualMedia;
        ssdp = builder.ssdp;
        telnet = builder.telnet;
        kvmip = builder.kvmip;
    }

    public static Builder newBuilder() {
        return new Builder();
    }

    public Id getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getDescription() {
        return description;
    }

    public Status getStatus() {
        return status;
    }

    public String getHostName() {
        return hostName;
    }

    public String getFqdn() {
        return fqdn;
    }

    public ProtocolDto getHttp() {
        return http;
    }

    public ProtocolDto getHttps() {
        return https;
    }

    public ProtocolDto getIpmi() {
        return ipmi;
    }

    public ProtocolDto getSsh() {
        return ssh;
    }

    public ProtocolDto getSnmp() {
        return snmp;
    }

    public ProtocolDto getVirtualMedia() {
        return virtualMedia;
    }

    public SimpleServiceDiscoveryProtocolDto getSsdp() {
        return ssdp;
    }

    public ProtocolDto getTelnet() {
        return telnet;
    }

    public ProtocolDto getKvmip() {
        return kvmip;
    }

    public static final class Builder {
        private Id id;
        private String name;
        private String description;
        private Status status;
        private String hostName;
        private String fqdn;
        private ProtocolDto http;
        private ProtocolDto https;
        private ProtocolDto ipmi;
        private ProtocolDto ssh;
        private ProtocolDto snmp;
        private ProtocolDto virtualMedia;
        private SimpleServiceDiscoveryProtocolDto ssdp;
        private ProtocolDto telnet;
        private ProtocolDto kvmip;

        private Builder() {
        }

        public Builder id(Id id) {
            this.id = id;
            return this;
        }

        public Builder name(String name) {
            this.name = name;
            return this;
        }

        public Builder description(String description) {
            this.description = description;
            return this;
        }

        public Builder status(Status status) {
            this.status = status;
            return this;
        }

        public Builder hostName(String hostName) {
            this.hostName = hostName;
            return this;
        }

        public Builder fqdn(String fqdn) {
            this.fqdn = fqdn;
            return this;
        }

        public Builder http(ProtocolDto http) {
            this.http = http;
            return this;
        }

        public Builder https(ProtocolDto https) {
            this.https = https;
            return this;
        }

        public Builder ipmi(ProtocolDto ipmi) {
            this.ipmi = ipmi;
            return this;
        }

        public Builder ssh(ProtocolDto ssh) {
            this.ssh = ssh;
            return this;
        }

        public Builder snmp(ProtocolDto snmp) {
            this.snmp = snmp;
            return this;
        }

        public Builder virtualMedia(ProtocolDto virtualMedia) {
            this.virtualMedia = virtualMedia;
            return this;
        }

        public Builder ssdp(SimpleServiceDiscoveryProtocolDto ssdp) {
            this.ssdp = ssdp;
            return this;
        }

        public Builder telnet(ProtocolDto telnet) {
            this.telnet = telnet;
            return this;
        }

        public Builder kvmip(ProtocolDto kvmip) {
            this.kvmip = kvmip;
            return this;
        }

        public NetworkProtocolDto build() {
            return new NetworkProtocolDto(this);
        }
    }
}
