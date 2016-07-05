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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.common.types.NotifyIpV6Scope;
import com.intel.podm.common.types.Status;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;

import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.REQUIRED;

@Dependent
@Transactional(REQUIRED)
public class NetworkProtocol extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> HOST_NAME = stringProperty("hostName");
    public static final DomainObjectProperty<String> FQDN = stringProperty("fqdn");
    public static final DomainObjectProperty<Boolean> HTTP_PROTOCOL_ENABLED = booleanProperty("httpProtocolEnabled");
    public static final DomainObjectProperty<Integer> HTTP_PORT = integerProperty("httpPort");
    public static final DomainObjectProperty<Boolean> HTTPS_PROTOCOL_ENABLED = booleanProperty("httpsProtocolEnabled");
    public static final DomainObjectProperty<Integer> HTTPS_PORT = integerProperty("httpsPort");
    public static final DomainObjectProperty<Boolean> IPMI_PROTOCOL_ENABLED = booleanProperty("ipmiProtocolEnabled");
    public static final DomainObjectProperty<Integer> IPMI_PORT = integerProperty("ipmiPort");
    public static final DomainObjectProperty<Boolean> SSH_PROTOCOL_ENABLED = booleanProperty("sshProtocolEnabled");
    public static final DomainObjectProperty<Integer> SSH_PORT = integerProperty("sshPort");
    public static final DomainObjectProperty<Boolean> SNMP_PROTOCOL_ENABLED = booleanProperty("snmpProtocolEnabled");
    public static final DomainObjectProperty<Integer> SNMP_PORT = integerProperty("snmpPort");
    public static final DomainObjectProperty<Boolean> VIRTUAL_MEDIA_PROTOCOL_ENABLED = booleanProperty("virtualMediaProtocolEnabled");
    public static final DomainObjectProperty<Integer> VIRTUAL_MEDIA_PORT = integerProperty("virtualMediaPort");
    public static final DomainObjectProperty<Boolean> SSDP_PROTOCOL_ENABLED = booleanProperty("ssdpProtocolEnabled");
    public static final DomainObjectProperty<Integer> SSDP_PORT = integerProperty("ssdpPort");
    public static final DomainObjectProperty<NotifyIpV6Scope> SSDP_NOTIFY_IP_V_6_SCOPE = enumProperty("ssdpNotifyIpV6Scope", NotifyIpV6Scope.class);
    public static final DomainObjectProperty<Integer> SSDP_NOTIFY_MULTICAST_INTERVAL_SECONDS = integerProperty("ssdpNotifyMulticastIntervalSeconds");
    public static final DomainObjectProperty<Integer> SSDP_NOTIFY_TTL = integerProperty("ssdpNotifyTtl");
    public static final DomainObjectProperty<Boolean> TELNET_PROTOCOL_ENABLED = booleanProperty("telnetProtocolEnabled");
    public static final DomainObjectProperty<Integer> TELNET_PORT = integerProperty("telnetPort");
    public static final DomainObjectProperty<Boolean> KVM_IP_PROTOCOL_ENABLED = booleanProperty("kvmIpProtocolEnabled");
    public static final DomainObjectProperty<Integer> KVM_IP_PORT = integerProperty("kvmIpPort");

    @Override
    public String getName() {
        return getProperty(NAME);
    }

    @Override
    public void setName(String name) {
        setProperty(NAME, name);
    }

    @Override
    public String getDescription() {
        return getProperty(DESCRIPTION);
    }

    @Override
    public void setDescription(String description) {
        setProperty(DESCRIPTION, description);
    }

    public Integer getKvmIpPort() {
        return getProperty(KVM_IP_PORT);
    }

    public void setKvmIpPort(Integer kvmIpPort) {
        setProperty(KVM_IP_PORT, kvmIpPort);
    }

    public Boolean getKvmIpProtocolEnabled() {
        return getProperty(KVM_IP_PROTOCOL_ENABLED);
    }

    public void setKvmIpProtocolEnabled(Boolean kvmIpProtocolEnabled) {
        setProperty(KVM_IP_PROTOCOL_ENABLED, kvmIpProtocolEnabled);
    }

    public Integer getTelnetPort() {
        return getProperty(TELNET_PORT);
    }

    public void setTelnetPort(Integer telnetPort) {
        setProperty(TELNET_PORT, telnetPort);
    }

    public Boolean getTelnetProtocolEnabled() {
        return getProperty(TELNET_PROTOCOL_ENABLED);
    }

    public void setTelnetProtocolEnabled(Boolean telnetProtocolEnabled) {
        setProperty(TELNET_PROTOCOL_ENABLED, telnetProtocolEnabled);
    }

    public Integer getSsdpNotifyTtl() {
        return getProperty(SSDP_NOTIFY_TTL);
    }

    public void setSsdpNotifyTtl(Integer ssdpNotifyTtl) {
        setProperty(SSDP_NOTIFY_TTL, ssdpNotifyTtl);
    }

    public Integer getSsdpNotifyMulticastIntervalSeconds() {
        return getProperty(SSDP_NOTIFY_MULTICAST_INTERVAL_SECONDS);
    }

    public void setSsdpNotifyMulticastIntervalSeconds(Integer ssdpNotifyMulticastIntervalSeconds) {
        setProperty(SSDP_NOTIFY_MULTICAST_INTERVAL_SECONDS, ssdpNotifyMulticastIntervalSeconds);
    }

    public NotifyIpV6Scope getSsdpNotifyIpV6Scope() {
        return getProperty(SSDP_NOTIFY_IP_V_6_SCOPE);
    }

    public void setSsdpNotifyIpV6Scope(NotifyIpV6Scope ssdpNotifyIpV6Scope) {
        setProperty(SSDP_NOTIFY_IP_V_6_SCOPE, ssdpNotifyIpV6Scope);
    }

    public Integer getSsdpPort() {
        return getProperty(SSDP_PORT);
    }

    public void setSsdpPort(Integer ssdpPort) {
        setProperty(SSDP_PORT, ssdpPort);
    }

    public Boolean getSsdpProtocolEnabled() {
        return getProperty(SSDP_PROTOCOL_ENABLED);
    }

    public void setSsdpProtocolEnabled(Boolean ssdpProtocolEnabled) {
        setProperty(SSDP_PROTOCOL_ENABLED, ssdpProtocolEnabled);
    }

    public Integer getVirtualMediaPort() {
        return getProperty(VIRTUAL_MEDIA_PORT);
    }

    public void setVirtualMediaPort(Integer virtualMediaPort) {
        setProperty(VIRTUAL_MEDIA_PORT, virtualMediaPort);
    }

    public Boolean getVirtualMediaProtocolEnabled() {
        return getProperty(VIRTUAL_MEDIA_PROTOCOL_ENABLED);
    }

    public void setVirtualMediaProtocolEnabled(Boolean virtualMediaProtocolEnabled) {
        setProperty(VIRTUAL_MEDIA_PROTOCOL_ENABLED, virtualMediaProtocolEnabled);
    }

    public Integer getSnmpPort() {
        return getProperty(SNMP_PORT);
    }

    public void setSnmpPort(Integer snmpPort) {
        setProperty(SNMP_PORT, snmpPort);
    }

    public Boolean getSnmpProtocolEnabled() {
        return getProperty(SNMP_PROTOCOL_ENABLED);
    }

    public void setSnmpProtocolEnabled(Boolean snmpProtocolEnabled) {
        setProperty(SNMP_PROTOCOL_ENABLED, snmpProtocolEnabled);
    }

    public Integer getSshPort() {
        return getProperty(SSH_PORT);
    }

    public void setSshPort(Integer sshPort) {
        setProperty(SSH_PORT, sshPort);
    }

    public Boolean getSshProtocolEnabled() {
        return getProperty(SSH_PROTOCOL_ENABLED);
    }

    public void setSshProtocolEnabled(Boolean sshProtocolEnabled) {
        setProperty(SSH_PROTOCOL_ENABLED, sshProtocolEnabled);
    }

    public Integer getIpmiPort() {
        return getProperty(IPMI_PORT);
    }

    public void setIpmiPort(Integer ipmiPort) {
        setProperty(IPMI_PORT, ipmiPort);
    }

    public Boolean getIpmiProtocolEnabled() {
        return getProperty(IPMI_PROTOCOL_ENABLED);
    }

    public void setIpmiProtocolEnabled(Boolean ipmiProtocolEnabled) {
        setProperty(IPMI_PROTOCOL_ENABLED, ipmiProtocolEnabled);
    }

    public Integer getHttpsPort() {
        return getProperty(HTTPS_PORT);
    }

    public void setHttpsPort(Integer httpsPort) {
        setProperty(HTTPS_PORT, httpsPort);
    }

    public Boolean getHttpsProtocolEnabled() {
        return getProperty(HTTPS_PROTOCOL_ENABLED);
    }

    public void setHttpsProtocolEnabled(Boolean httpsProtocolEnabled) {
        setProperty(HTTPS_PROTOCOL_ENABLED, httpsProtocolEnabled);
    }

    public Integer getHttpPort() {
        return getProperty(HTTP_PORT);
    }

    public void setHttpPort(Integer httpPort) {
        setProperty(HTTP_PORT, httpPort);
    }

    public Boolean getHttpProtocolEnabled() {
        return getProperty(HTTP_PROTOCOL_ENABLED);
    }

    public void setHttpProtocolEnabled(Boolean httpProtocolEnabled) {
        setProperty(HTTP_PROTOCOL_ENABLED, httpProtocolEnabled);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public String getHostName() {
        return getProperty(HOST_NAME);
    }

    public void setHostName(String hostName) {
        setProperty(HOST_NAME, hostName);
    }

    public String getFqdn() {
        return getProperty(FQDN);
    }

    public void setFqdn(String fqdn) {
        setProperty(FQDN, fqdn);
    }

    @Override
    public URI getSourceUri() {
        return getProperty(SOURCE_URI);
    }

    @Override
    public void setSourceUri(URI sourceUri) {
        setProperty(SOURCE_URI, sourceUri);
    }

    @Override
    public ExternalService getService() {
        return singleOrNull(getLinked(OWNED_BY, ExternalService.class));
    }
}
