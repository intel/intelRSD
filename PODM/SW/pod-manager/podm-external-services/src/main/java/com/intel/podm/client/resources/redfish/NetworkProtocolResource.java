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

package com.intel.podm.client.resources.redfish;

import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.OdataTypes;
import com.intel.podm.client.resources.ExternalServiceResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

import static com.intel.podm.common.types.redfish.OdataTypeVersions.VERSION_PATTERN;

@OdataTypes({
    "#ManagerNetworkProtocol" + VERSION_PATTERN + "ManagerNetworkProtocol",
    "#ManagerNetworkService" + VERSION_PATTERN + "ManagerNetworkService",
    "#NetworkService" + VERSION_PATTERN + "NetworkService"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class NetworkProtocolResource extends ExternalServiceResourceImpl implements ExternalServiceResource {
    @JsonProperty("Status")
    private Status status;
    @JsonProperty("HostName")
    private String hostName;
    @JsonProperty("FQDN")
    private String fqdn;
    @JsonProperty("HTTP")
    private ProtocolObject http = new ProtocolObject();
    @JsonProperty("HTTPS")
    private ProtocolObject https = new ProtocolObject();
    @JsonProperty("IPMI")
    private ProtocolObject ipmi = new ProtocolObject();
    @JsonProperty("SSH")
    private ProtocolObject ssh = new ProtocolObject();
    @JsonProperty("SNMP")
    private ProtocolObject snmp = new ProtocolObject();
    @JsonProperty("VirtualMedia")
    private ProtocolObject virtualMedia = new ProtocolObject();
    @JsonProperty("SSDP")
    private SimpleServiceDiscoveryProtocolObject ssdp = new SimpleServiceDiscoveryProtocolObject();
    @JsonProperty("Telnet")
    private ProtocolObject telnet = new ProtocolObject();
    @JsonProperty("KVMIP")
    private ProtocolObject kvmip = new ProtocolObject();

    public Status getStatus() {
        return status;
    }

    public String getHostName() {
        return hostName;
    }

    public String getFqdn() {
        return fqdn;
    }

    public Integer getKvmIpPort() {
        return kvmip.getPort();
    }

    public Boolean getKvmIpProtocolEnabled() {
        return kvmip.isProtocolEnabled();
    }

    public Integer getTelnetPort() {
        return telnet.getPort();
    }

    public Boolean getTelnetProtocolEnabled() {
        return telnet.isProtocolEnabled();
    }

    public Integer getSsdpNotifyTtl() {
        return ssdp.getNotifyTtl();
    }

    public Integer getSsdpNotifyMulticastIntervalSeconds() {
        return ssdp.getNotifyMulticastIntervalSeconds();
    }

    public NotifyIpV6Scope getSsdpNotifyIpV6Scope() {
        return ssdp.getNotifyIpV6Scope();
    }

    public Integer getSsdpPort() {
        return ssdp.getPort();
    }

    public Boolean getSsdpProtocolEnabled() {
        return ssdp.isProtocolEnabled();
    }

    public Integer getVirtualMediaPort() {
        return virtualMedia.getPort();
    }

    public Boolean getVirtualMediaProtocolEnabled() {
        return virtualMedia.isProtocolEnabled();
    }

    public Integer getSnmpPort() {
        return snmp.getPort();
    }

    public Boolean getSnmpProtocolEnabled() {
        return snmp.isProtocolEnabled();
    }

    public Integer getSshPort() {
        return ssh.getPort();
    }

    public Boolean getSshProtocolEnabled() {
        return ssh.isProtocolEnabled();
    }

    public Integer getIpmiPort() {
        return ipmi.getPort();
    }

    public Boolean getIpmiProtocolEnabled() {
        return ipmi.isProtocolEnabled();
    }

    public Integer getHttpsPort() {
        return https.getPort();
    }

    public Boolean getHttpsProtocolEnabled() {
        return https.isProtocolEnabled();
    }

    public Integer getHttpPort() {
        return http.getPort();
    }

    public Boolean getHttpProtocolEnabled() {
        return http.isProtocolEnabled();
    }
}
