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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.Eventable;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.NotifyIpV6Scope;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.Objects;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "network_protocol", indexes = @Index(name = "idx_network_protocol_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class NetworkProtocol extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "kvm_ip_port")
    private Integer kvmIpPort;

    @Column(name = "kvm_ip_protocol_enabled")
    private Boolean kvmIpProtocolEnabled;

    @Column(name = "telnet_port")
    private Integer telnetPort;

    @Column(name = "telnet_protocol_enabled")
    private Boolean telnetProtocolEnabled;

    @Column(name = "ssdp_notify_ttl")
    private Integer ssdpNotifyTtl;

    @Column(name = "ssdp_notify_multicast_interval_seconds")
    private Integer ssdpNotifyMulticastIntervalSeconds;

    @Column(name = "ssdp_notify_ipv6_scope")
    @Enumerated(STRING)
    private NotifyIpV6Scope ssdpNotifyIpV6Scope;

    @Column(name = "ssdp_port")
    private Integer ssdpPort;

    @Column(name = "ssdp_protocol_enabled")
    private Boolean ssdpProtocolEnabled;

    @Column(name = "virtual_media_port")
    private Integer virtualMediaPort;

    @Column(name = "virtual_media_protocol_enabled")
    private Boolean virtualMediaProtocolEnabled;

    @Column(name = "snmp_port")
    private Integer snmpPort;

    @Column(name = "snmp_protocol_enabled")
    private Boolean snmpProtocolEnabled;

    @Column(name = "ssh_port")
    private Integer sshPort;

    @Column(name = "ssh_protocol_enabled")
    private Boolean sshProtocolEnabled;

    @Column(name = "ipmi_port")
    private Integer ipmiPort;

    @Column(name = "ipmi_protocol_enabled")
    private Boolean ipmiProtocolEnabled;

    @Column(name = "http_port")
    private Integer httpPort;

    @Column(name = "http_protocol_enabled")
    private Boolean httpProtocolEnabled;

    @Column(name = "https_port")
    private Integer httpsPort;

    @Column(name = "https_protocol_enabled")
    private Boolean httpsProtocolEnabled;

    @Column(name = "hostname")
    private String hostname;

    @Column(name = "fqdn")
    private String fqdn;

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "manager_id")
    private Manager manager;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public Integer getKvmIpPort() {
        return kvmIpPort;
    }

    public void setKvmIpPort(Integer kvmIpPort) {
        this.kvmIpPort = kvmIpPort;
    }

    public Boolean getKvmIpProtocolEnabled() {
        return kvmIpProtocolEnabled;
    }

    public void setKvmIpProtocolEnabled(Boolean kvmIpProtocolEnabled) {
        this.kvmIpProtocolEnabled = kvmIpProtocolEnabled;
    }

    public Integer getTelnetPort() {
        return telnetPort;
    }

    public void setTelnetPort(Integer telnetPort) {
        this.telnetPort = telnetPort;
    }

    public Boolean getTelnetProtocolEnabled() {
        return telnetProtocolEnabled;
    }

    public void setTelnetProtocolEnabled(Boolean telnetProtocolEnabled) {
        this.telnetProtocolEnabled = telnetProtocolEnabled;
    }

    public Integer getSsdpNotifyTtl() {
        return ssdpNotifyTtl;
    }

    public void setSsdpNotifyTtl(Integer ssdpNotifyTtl) {
        this.ssdpNotifyTtl = ssdpNotifyTtl;
    }

    public Integer getSsdpNotifyMulticastIntervalSeconds() {
        return ssdpNotifyMulticastIntervalSeconds;
    }

    public void setSsdpNotifyMulticastIntervalSeconds(Integer ssdpNotifyMulticastIntervalSeconds) {
        this.ssdpNotifyMulticastIntervalSeconds = ssdpNotifyMulticastIntervalSeconds;
    }

    public NotifyIpV6Scope getSsdpNotifyIpV6Scope() {
        return ssdpNotifyIpV6Scope;
    }

    public void setSsdpNotifyIpV6Scope(NotifyIpV6Scope ssdpNotifyIpV6Scope) {
        this.ssdpNotifyIpV6Scope = ssdpNotifyIpV6Scope;
    }

    public Integer getSsdpPort() {
        return ssdpPort;
    }

    public void setSsdpPort(Integer ssdpPort) {
        this.ssdpPort = ssdpPort;
    }

    public Boolean getSsdpProtocolEnabled() {
        return ssdpProtocolEnabled;
    }

    public void setSsdpProtocolEnabled(Boolean ssdpProtocolEnabled) {
        this.ssdpProtocolEnabled = ssdpProtocolEnabled;
    }

    public Integer getVirtualMediaPort() {
        return virtualMediaPort;
    }

    public void setVirtualMediaPort(Integer virtualMediaPort) {
        this.virtualMediaPort = virtualMediaPort;
    }

    public Boolean getVirtualMediaProtocolEnabled() {
        return virtualMediaProtocolEnabled;
    }

    public void setVirtualMediaProtocolEnabled(Boolean virtualMediaProtocolEnabled) {
        this.virtualMediaProtocolEnabled = virtualMediaProtocolEnabled;
    }

    public Integer getSnmpPort() {
        return snmpPort;
    }

    public void setSnmpPort(Integer snmpPort) {
        this.snmpPort = snmpPort;
    }

    public Boolean getSnmpProtocolEnabled() {
        return snmpProtocolEnabled;
    }

    public void setSnmpProtocolEnabled(Boolean snmpProtocolEnabled) {
        this.snmpProtocolEnabled = snmpProtocolEnabled;
    }

    public Integer getSshPort() {
        return sshPort;
    }

    public void setSshPort(Integer sshPort) {
        this.sshPort = sshPort;
    }

    public Boolean getSshProtocolEnabled() {
        return sshProtocolEnabled;
    }

    public void setSshProtocolEnabled(Boolean sshProtocolEnabled) {
        this.sshProtocolEnabled = sshProtocolEnabled;
    }

    public Integer getIpmiPort() {
        return ipmiPort;
    }

    public void setIpmiPort(Integer ipmiPort) {
        this.ipmiPort = ipmiPort;
    }

    public Boolean getIpmiProtocolEnabled() {
        return ipmiProtocolEnabled;
    }

    public void setIpmiProtocolEnabled(Boolean ipmiProtocolEnabled) {
        this.ipmiProtocolEnabled = ipmiProtocolEnabled;
    }

    public Integer getHttpPort() {
        return httpPort;
    }

    public void setHttpPort(Integer httpPort) {
        this.httpPort = httpPort;
    }

    public Boolean getHttpProtocolEnabled() {
        return httpProtocolEnabled;
    }

    public void setHttpProtocolEnabled(Boolean httpProtocolEnabled) {
        this.httpProtocolEnabled = httpProtocolEnabled;
    }

    public Integer getHttpsPort() {
        return httpsPort;
    }

    public void setHttpsPort(Integer httpsPort) {
        this.httpsPort = httpsPort;
    }

    public Boolean getHttpsProtocolEnabled() {
        return httpsProtocolEnabled;
    }

    public void setHttpsProtocolEnabled(Boolean httpsProtocolEnabled) {
        this.httpsProtocolEnabled = httpsProtocolEnabled;
    }

    public String getHostName() {
        return hostname;
    }

    public void setHostName(String hostName) {
        this.hostname = hostName;
    }

    public String getFqdn() {
        return fqdn;
    }

    public void setFqdn(String fqdn) {
        this.fqdn = fqdn;
    }

    public Manager getManager() {
        return manager;
    }

    public void setManager(Manager manager) {
        if (!Objects.equals(this.manager, manager)) {
            unlinkManager(this.manager);
            this.manager = manager;
            if (manager != null && !this.equals(manager.getNetworkProtocol())) {
                manager.setNetworkProtocol(this);
            }
        }
    }

    public void unlinkManager(Manager manager) {
        if (Objects.equals(this.manager, manager)) {
            this.manager = null;
            if (manager != null) {
                manager.unlinkNetworkProtocol(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkManager(manager);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
