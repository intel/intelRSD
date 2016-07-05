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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.business.entities.redfish.base.Descriptable;
import com.intel.podm.business.entities.redfish.base.Discoverable;
import com.intel.podm.business.entities.redfish.base.StatusPossessor;
import com.intel.podm.business.entities.redfish.properties.IpV4Address;
import com.intel.podm.business.entities.redfish.properties.IpV6Address;
import com.intel.podm.business.entities.redfish.properties.IpV6AddressPolicy;
import com.intel.podm.business.entities.redfish.properties.NameServer;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.IPV6_ADDRESS_POLICY;
import static com.intel.podm.business.entities.base.DomainObjectLink.IPV6_STATIC_ADDRESS;
import static com.intel.podm.business.entities.base.DomainObjectLink.IP_ADDRESS;
import static com.intel.podm.business.entities.base.DomainObjectLink.NAME_SERVER;
import static com.intel.podm.business.entities.base.DomainObjectLink.NEIGHBOR_SWITCH_PORT;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.macAddressProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class EthernetInterface extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<Boolean> INTERFACE_ENABLED = booleanProperty("interfaceEnabled");
    public static final DomainObjectProperty<Boolean> AUTO_NEG = booleanProperty("autoNeg");
    public static final DomainObjectProperty<Integer> MTU_SIZE = integerProperty("mtuSize");
    public static final DomainObjectProperty<Integer> MAX_IPV_6_STATIC_ADDRESSES = integerProperty("maxIPv6StaticAddresses");
    public static final DomainObjectProperty<Boolean> VLAN_ENABLE = booleanProperty("vlanEnable");
    public static final DomainObjectProperty<Integer> VLAN_ID = integerProperty("vlanId");
    public static final DomainObjectProperty<MacAddress> PERMANENT_MAC_ADDRESS = macAddressProperty("permanentMacAddress");
    public static final DomainObjectProperty<MacAddress> MAC_ADDRESS = macAddressProperty("macAddress");
    public static final DomainObjectProperty<Integer> SPEED_MBPS = integerProperty("speedMbps");
    public static final DomainObjectProperty<Boolean> FULL_DUPLEX = booleanProperty("fullDuplex");
    public static final DomainObjectProperty<String> HOST_NAME = stringProperty("hostName");
    public static final DomainObjectProperty<String> FQDN = stringProperty("fqdn");
    public static final DomainObjectProperty<String> IP_V6_DEFAULT_GATEWAY = stringProperty("ipV6DefaultGateway");

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

    public MacAddress getPermanentMacAddress() {
        return getProperty(PERMANENT_MAC_ADDRESS);
    }

    public void setPermanentMacAddress(MacAddress factoryMacAddress) {
        setProperty(PERMANENT_MAC_ADDRESS, factoryMacAddress);
    }

    public MacAddress getMacAddress() {
        return getProperty(MAC_ADDRESS);
    }

    public void setMacAddress(MacAddress macAddress) {
        setProperty(MAC_ADDRESS, macAddress);
    }

    public Integer getSpeedMbps() {
        return getProperty(SPEED_MBPS);
    }

    public void setSpeedMbps(Integer speedMbps) {
        setProperty(SPEED_MBPS, speedMbps);
    }

    public Boolean isFullDuplex() {
        return getProperty(FULL_DUPLEX);
    }

    public void setFullDuplex(Boolean fullDuplex) {
        setProperty(FULL_DUPLEX, fullDuplex);
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

    public Boolean getInterfaceEnabled() {
        return getProperty(INTERFACE_ENABLED);
    }

    public void setInterfaceEnabled(Boolean interfaceEnabled) {
        setProperty(INTERFACE_ENABLED, interfaceEnabled);
    }


    public Boolean getAutoNeg() {
        return getProperty(AUTO_NEG);
    }

    public void setAutoNeg(Boolean autoNeg) {
        setProperty(AUTO_NEG, autoNeg);
    }


    public Integer getMtuSize() {
        return getProperty(MTU_SIZE);
    }

    public void setMtuSize(Integer mtuSize) {
        setProperty(MTU_SIZE, mtuSize);
    }

    public Boolean getVlanEnable() {
        return getProperty(VLAN_ENABLE);
    }

    public void setVlanEnable(Boolean vlanEnable) {
        setProperty(VLAN_ENABLE, vlanEnable);
    }

    public Integer getVlanId() {
        return getProperty(VLAN_ID);
    }

    public void setVlanId(Integer vlanId) {
        setProperty(VLAN_ID, vlanId);
    }

    public void addVlan(EthernetSwitchPortVlan vlan) {
        link(CONTAINS, vlan);
    }

    public Collection<EthernetSwitchPortVlan> getVlans() {
        return getLinked(CONTAINS, EthernetSwitchPortVlan.class);
    }

    public Integer getMaxIPv6StaticAddresses() {
        return getProperty(MAX_IPV_6_STATIC_ADDRESSES);
    }

    public void setMaxIPv6StaticAddresses(Integer maxIPv6StaticAddresses) {
        setProperty(MAX_IPV_6_STATIC_ADDRESSES, maxIPv6StaticAddresses);
    }

    public String getIpV6DefaultGateway() {
        return getProperty(IP_V6_DEFAULT_GATEWAY);
    }

    public void setIpV6DefaultGateway(String ipV6DefaultGateway) {
        setProperty(IP_V6_DEFAULT_GATEWAY, ipV6DefaultGateway);
    }

    public IpV4Address addIpV4Address() {
        return addDomainObject(IP_ADDRESS, IpV4Address.class);
    }

    public Collection<IpV4Address> getIpV4Addresses() {
        return getLinked(IP_ADDRESS, IpV4Address.class);
    }

    public void setIpV4Addresses(Collection<IpV4Address> addresses) {
        addresses.forEach(addr -> link(IP_ADDRESS, addr));
    }

    public List<IpV6Address> getIpV6Addresses() {
        return getLinked(IP_ADDRESS, IpV6Address.class);
    }

    public void setIpV6Addresses(Collection<IpV6Address> addresses) {
        addresses.forEach(addr -> link(IP_ADDRESS, addr));
    }

    public List<IpV6Address> getIpV6StaticAddresses() {
        return getLinked(IPV6_STATIC_ADDRESS, IpV6Address.class);
    }

    public void setIpV6StaticAddresses(List<IpV6Address> staticAddresses) {
        staticAddresses.forEach(staticAddress -> link(IPV6_STATIC_ADDRESS, staticAddress));
    }

    public List<IpV6AddressPolicy> getIpV6AddressesPolicyTable() {
        return getLinked(IPV6_ADDRESS_POLICY, IpV6AddressPolicy.class);
    }

    public void setIpV6AddressesPolicyTable(List<IpV6AddressPolicy> policyList) {
        policyList.forEach(policy -> link(IPV6_ADDRESS_POLICY, policy));
    }

    public NameServer addNameServer() {
        return addDomainObject(NAME_SERVER, NameServer.class);
    }

    public Collection<NameServer> getNameServers() {
        return getLinked(NAME_SERVER, NameServer.class);
    }

    public void setNameServers(Collection<NameServer> nameServers) {
        nameServers.forEach(nameServer -> link(NAME_SERVER, nameServer));
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
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

    public ComputerSystem getComputerSystem() {
        return singleOrNull(getLinked(CONTAINED_BY, ComputerSystem.class));
    }

    public EthernetSwitchPort getNeighborSwitchPort() {
        return singleOrNull(getLinked(NEIGHBOR_SWITCH_PORT, EthernetSwitchPort.class));
    }

    public void unlinkEthernetSwitchPort() {
        for (EthernetSwitchPort switchPort : getLinked(NEIGHBOR_SWITCH_PORT, EthernetSwitchPort.class)) {
            unlink(NEIGHBOR_SWITCH_PORT, switchPort);
        }
    }
}
