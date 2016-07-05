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
import com.intel.podm.business.entities.redfish.properties.IpV4Address;
import com.intel.podm.business.entities.redfish.properties.IpV6Address;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.net.MacAddress;

import javax.enterprise.context.Dependent;
import javax.transaction.Transactional;
import java.net.URI;
import java.util.Collection;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINS;
import static com.intel.podm.business.entities.base.DomainObjectLink.IP_ADDRESS;
import static com.intel.podm.business.entities.base.DomainObjectLink.MEMBER_OF_PORT;
import static com.intel.podm.business.entities.base.DomainObjectLink.NEIGHBOR_ETHERNET_INTERFACE;
import static com.intel.podm.business.entities.base.DomainObjectLink.OWNED_BY;
import static com.intel.podm.business.entities.base.DomainObjectLink.PORT_MEMBERS;
import static com.intel.podm.business.entities.base.DomainObjectLink.PRIMARY_VLAN;
import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.enumProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.integerProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.macAddressProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.neighborInfoProperty;
import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.utils.IterableHelper.single;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
@Transactional(MANDATORY)
public class EthernetSwitchPort extends DomainObject implements Discoverable, StatusPossessor, Descriptable {
    public static final DomainObjectProperty<String> PORT_ID = stringProperty("portId");
    public static final DomainObjectProperty<LinkType> LINK_TYPE = enumProperty("linkType", LinkType.class);
    public static final DomainObjectProperty<OperationalState> OPERATIONAL_STATE = enumProperty("operationalState", OperationalState.class);
    public static final DomainObjectProperty<AdministrativeState> ADMINISTRATIVE_STATE = enumProperty("administrativeState", AdministrativeState.class);
    public static final DomainObjectProperty<Integer> LINK_SPEED_MBPS = integerProperty("linkSpeedMbps");
    public static final DomainObjectProperty<NeighborInfo> NEIGHBOR_INFO = neighborInfoProperty("neighborInfo");
    public static final DomainObjectProperty<MacAddress> NEIGHBOR_MAC = macAddressProperty("neighborMac");
    public static final DomainObjectProperty<Boolean> AUTOSENSE = booleanProperty("autosense");
    public static final DomainObjectProperty<Integer> FRAME_SIZE = integerProperty("frameSize");
    public static final DomainObjectProperty<Boolean> FULL_DUPLEX = booleanProperty("fullDuplex");
    public static final DomainObjectProperty<MacAddress> MAC_ADDRESS = macAddressProperty("macAddress");
    public static final DomainObjectProperty<PortClass> PORT_CLASS = enumProperty("portClass", PortClass.class);
    public static final DomainObjectProperty<PortMode> PORT_MODE = enumProperty("portMode", PortMode.class);
    public static final DomainObjectProperty<PortType> PORT_TYPE = enumProperty("portType", PortType.class);

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

    public String getPortId() {
        return getProperty(PORT_ID);
    }

    public void setPortId(String value) {
        setProperty(PORT_ID, value);
    }

    @Override
    public Status getStatus() {
        return getProperty(STATUS);
    }

    @Override
    public void setStatus(Status status) {
        setProperty(STATUS, status);
    }

    public Integer getLinkSpeedMbps() {
        return getProperty(LINK_SPEED_MBPS);
    }

    public void setLinkSpeedMbps(Integer value) {
        setProperty(LINK_SPEED_MBPS, value);
    }

    public OperationalState getOperationalState() {
        return getProperty(OPERATIONAL_STATE);
    }

    public void setOperationalState(OperationalState value) {
        setProperty(OPERATIONAL_STATE, value);
    }

    public AdministrativeState getAdministrativeState() {
        return getProperty(ADMINISTRATIVE_STATE);
    }

    public void setAdministrativeState(AdministrativeState value) {
        setProperty(ADMINISTRATIVE_STATE, value);
    }

    public LinkType getLinkType() {
        return getProperty(LINK_TYPE);
    }

    public void setLinkType(LinkType value) {
        setProperty(LINK_TYPE, value);
    }

    public NeighborInfo getNeighborInfo() {
        return getProperty(NEIGHBOR_INFO);
    }

    public void setNeighborInfo(NeighborInfo neighborInfo) {
        setProperty(NEIGHBOR_INFO, neighborInfo);
    }

    public MacAddress getNeighborMac() {
        return getProperty(NEIGHBOR_MAC);
    }

    public void setNeighborMac(MacAddress macAddress) {
        setProperty(NEIGHBOR_MAC, macAddress);
    }

    public Boolean getAutosense() {
        return getProperty(AUTOSENSE);
    }

    public void setAutosense(Boolean autosense) {
        setProperty(AUTOSENSE, autosense);
    }

    public Boolean getFullDuplex() {
        return getProperty(FULL_DUPLEX);
    }

    public void setFullDuplex(Boolean fullDuplex) {
        setProperty(FULL_DUPLEX, fullDuplex);
    }

    public Integer getFrameSize() {
        return getProperty(FRAME_SIZE);
    }

    public void setFrameSize(Integer frameSize) {
        setProperty(FRAME_SIZE, frameSize);
    }

    public MacAddress getMacAddress() {
        return getProperty(MAC_ADDRESS);
    }

    public void setMacAddress(MacAddress macAddress) {
        setProperty(MAC_ADDRESS, macAddress);
    }

    public PortClass getPortClass() {
        return getProperty(PORT_CLASS);
    }

    public void setPortClass(PortClass portClass) {
        setProperty(PORT_CLASS, portClass);
    }

    public PortMode getPortMode() {
        return getProperty(PORT_MODE);
    }

    public void setPortMode(PortMode portMode) {
        setProperty(PORT_MODE, portMode);
    }

    public PortType getPortType() {
        return getProperty(PORT_TYPE);
    }

    public void setPortType(PortType portType) {
        setProperty(PORT_TYPE, portType);
    }

    public void setPrimaryVlan(EthernetSwitchPortVlan primaryVlan) {
        setLink(PRIMARY_VLAN, primaryVlan);
    }

    public void removePrimaryVlan(EthernetSwitchPortVlan primaryVlan) {
        unlink(PRIMARY_VLAN, primaryVlan);
    }

    public EthernetSwitchPortVlan getPrimaryVlan() {
        return singleOrNull(getLinked(PRIMARY_VLAN, EthernetSwitchPortVlan.class));
    }

    public void addVlan(EthernetSwitchPortVlan vlan) {
        link(CONTAINS, vlan);
    }

    public Collection<EthernetSwitchPortVlan> getVlans() {
        return getLinked(CONTAINS, EthernetSwitchPortVlan.class);
    }

    public EthernetSwitch getEthernetSwitch() {
        return single(getLinked(CONTAINED_BY, EthernetSwitch.class));
    }

    public void setIpV4Addresses(Collection<IpV4Address> addresses) {
        addresses.forEach(address -> link(IP_ADDRESS, address));
    }

    public void setIpV6Addresses(Collection<IpV6Address> addresses) {
        addresses.forEach(address -> link(IP_ADDRESS, address));
    }

    public Collection<IpV4Address> getIpV4Addresses() {
        return getLinked(IP_ADDRESS, IpV4Address.class);
    }

    public Collection<IpV6Address> getIpV6Addresses() {
        return getLinked(IP_ADDRESS, IpV6Address.class);
    }

    public EthernetSwitchPort getMemberOfPort() {
        try {
            return singleOrNull(getLinked(MEMBER_OF_PORT, EthernetSwitchPort.class));
        } catch (IllegalStateException e) {
            throw new IllegalStateException("EthernetSwitchPort may be a member of at most one port", e);
        }
    }

    public Collection<EthernetSwitchPort> getPortMembers() {
        return getLinked(PORT_MEMBERS, EthernetSwitchPort.class);
    }

    public void addPortMember(EthernetSwitchPort switchPort) {
        link(PORT_MEMBERS, switchPort);
    }

    public void removePortMember(EthernetSwitchPort switchPort) {
        unlink(PORT_MEMBERS, switchPort);
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

    public void linkEthernetInterface(EthernetInterface ethernetInterface) {
        link(NEIGHBOR_ETHERNET_INTERFACE, ethernetInterface);
    }

    public void unlinkEthernetInterfaces() {
        for (EthernetInterface ethernetInterface : getLinked(NEIGHBOR_ETHERNET_INTERFACE, EthernetInterface.class)) {
            unlink(NEIGHBOR_ETHERNET_INTERFACE, ethernetInterface);
        }
    }

    public EthernetInterface getNeighborInterface() {
        return singleOrNull(getLinked(NEIGHBOR_ETHERNET_INTERFACE, EthernetInterface.class));
    }
}
