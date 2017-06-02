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
import com.intel.podm.client.api.resources.redfish.NetworkProtocolResource;
import com.intel.podm.client.resources.ExternalServiceResourceImpl;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

@OdataTypes({
        "#ManagerNetworkProtocol" + OdataTypes.VERSION_PATTERN + "ManagerNetworkProtocol",
        "#ManagerNetworkService" + OdataTypes.VERSION_PATTERN + "ManagerNetworkService",
        "#NetworkService" + OdataTypes.VERSION_PATTERN + "NetworkService"
})
@SuppressWarnings({"checkstyle:MethodCount"})
public class NetworkProtocolResourceImpl extends ExternalServiceResourceImpl implements NetworkProtocolResource {
    @JsonProperty ("Status")
    private Status status;
    @JsonProperty ("HostName")
    private String hostName;
    @JsonProperty ("FQDN")
    private String fqdn;
    @JsonProperty("HTTP")
    private ProtocolObjectImpl http = new ProtocolObjectImpl();
    @JsonProperty("HTTPS")
    private ProtocolObjectImpl https = new ProtocolObjectImpl();
    @JsonProperty("IPMI")
    private ProtocolObjectImpl ipmi = new ProtocolObjectImpl();
    @JsonProperty("SSH")
    private ProtocolObjectImpl ssh = new ProtocolObjectImpl();
    @JsonProperty("SNMP")
    private ProtocolObjectImpl snmp = new ProtocolObjectImpl();
    @JsonProperty("VirtualMedia")
    private ProtocolObjectImpl virtualMedia = new ProtocolObjectImpl();
    @JsonProperty("SSDP")
    private SimpleServiceDiscoveryProtocolObjectImpl ssdp = new SimpleServiceDiscoveryProtocolObjectImpl();
    @JsonProperty("Telnet")
    private ProtocolObjectImpl telnet = new ProtocolObjectImpl();
    @JsonProperty("KVMIP")
    private ProtocolObjectImpl kvmip = new ProtocolObjectImpl();

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public String getHostName() {
        return hostName;
    }

    @Override
    public String getFqdn() {
        return fqdn;
    }

    @Override
    public Integer getKvmIpPort() {
        return kvmip.getPort();
    }

    @Override
    public Boolean getKvmIpProtocolEnabled() {
        return kvmip.isProtocolEnabled();
    }

    @Override
    public Integer getTelnetPort() {
        return telnet.getPort();
    }

    @Override
    public Boolean getTelnetProtocolEnabled() {
        return telnet.isProtocolEnabled();
    }

    @Override
    public Integer getSsdpNotifyTtl() {
        return ssdp.getNotifyTtl();
    }

    @Override
    public Integer getSsdpNotifyMulticastIntervalSeconds() {
        return ssdp.getNotifyMulticastIntervalSeconds();
    }

    @Override
    public NotifyIpV6Scope getSsdpNotifyIpV6Scope() {
        return ssdp.getNotifyIpV6Scope();
    }

    @Override
    public Integer getSsdpPort() {
        return ssdp.getPort();
    }

    @Override
    public Boolean getSsdpProtocolEnabled() {
        return ssdp.isProtocolEnabled();
    }

    @Override
    public Integer getVirtualMediaPort() {
        return virtualMedia.getPort();
    }

    @Override
    public Boolean getVirtualMediaProtocolEnabled() {
        return virtualMedia.isProtocolEnabled();
    }

    @Override
    public Integer getSnmpPort() {
        return snmp.getPort();
    }

    @Override
    public Boolean getSnmpProtocolEnabled() {
        return snmp.isProtocolEnabled();
    }

    @Override
    public Integer getSshPort() {
        return ssh.getPort();
    }

    @Override
    public Boolean getSshProtocolEnabled() {
        return ssh.isProtocolEnabled();
    }

    @Override
    public Integer getIpmiPort() {
        return ipmi.getPort();
    }

    @Override
    public Boolean getIpmiProtocolEnabled() {
        return ipmi.isProtocolEnabled();
    }

    @Override
    public Integer getHttpsPort() {
        return https.getPort();
    }

    @Override
    public Boolean getHttpsProtocolEnabled() {
        return https.isProtocolEnabled();
    }

    @Override
    public Integer getHttpPort() {
        return http.getPort();
    }

    @Override
    public Boolean getHttpProtocolEnabled() {
        return http.isProtocolEnabled();
    }
}
