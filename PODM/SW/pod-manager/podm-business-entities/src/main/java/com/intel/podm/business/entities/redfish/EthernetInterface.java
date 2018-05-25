/*
 * Copyright (c) 2015-2018 Intel Corporation
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
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.MultiSourceResource;
import com.intel.podm.business.entities.redfish.base.VlanPossessor;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6AddressPolicy;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.LinkStatus;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.net.MacAddress;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = EthernetInterface.GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS,
        query = "SELECT ei FROM EthernetInterface ei WHERE ei.macAddress = :macAddress"
    ),
    @NamedQuery(name = EthernetInterface.GET_ETHERNET_INTERFACE_MULTI_SOURCE,
        query = "SELECT ethernetInterface "
            + "FROM EthernetInterface ethernetInterface "
            + "WHERE ethernetInterface.multiSourceDiscriminator = :multiSourceDiscriminator "
            + "AND ethernetInterface.isComplementary = :isComplementary"
    )
})
@Table(name = "ethernet_interface", indexes = @Index(name = "idx_ethernet_interface_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount", "checkstyle:ClassFanOutComplexity"})
public class EthernetInterface extends DiscoverableEntity implements VlanPossessor, MultiSourceResource {
    public static final String GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS = "GET_ETHERNET_INTERFACE_BY_MAC_ADDRESS";
    public static final String GET_ETHERNET_INTERFACE_MULTI_SOURCE = "GET_ETHERNET_INTERFACE_MULTI_SOURCE";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "link_status")
    @Enumerated(STRING)
    private LinkStatus linkStatus;

    @Column(name = "interface_enabled")
    private Boolean interfaceEnabled;

    @Column(name = "auto_neg")
    private Boolean autoNeg;

    @Column(name = "mtu_size")
    private Integer mtuSize;

    @Column(name = "max_ipv6_static_addresses")
    private Integer maxIPv6StaticAddresses;

    @Column(name = "vlan_enable")
    private Boolean vlanEnable;

    @Column(name = "vlan_id")
    private Integer vlanId;

    @Column(name = "permanent_mac_address")
    private MacAddress permanentMacAddress;

    @Column(name = "mac_address")
    private MacAddress macAddress;

    @Column(name = "speed_mbps")
    private Integer speedMbps;

    @Column(name = "full_duplex")
    private Boolean fullDuplex;

    @Column(name = "hostname")
    private String hostName;

    @Column(name = "fqdn")
    private String fqdn;

    @Column(name = "ipv6_default_gateway")
    private String ipV6DefaultGateway;

    @Column(name = "multi_source_discriminator")
    private String multiSourceDiscriminator;

    @ElementCollection
    @CollectionTable(name = "ethernet_interface_name_server", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @Column(name = "name_server")
    @OrderColumn(name = "name_server_order")
    private List<String> nameServers = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "ethernet_interface_ipv4_address", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @OrderColumn(name = "ipv4_address_order")
    private List<IpV4Address> ipV4Addresses = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "ethernet_interface_ipv6_address", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @OrderColumn(name = "ipv6_address_order")
    private List<IpV6Address> ipV6Addresses = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "ethernet_interface_ipv6_static_address", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @OrderColumn(name = "ipv6_static_address_order")
    private List<IpV6Address> ipV6StaticAddresses = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "ethernet_interface_ipv6_address_policy", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @OrderColumn(name = "ipv6_address_policy_order")
    private List<IpV6AddressPolicy> ipV6AddressPolicyTable = new ArrayList<>();

    @SuppressEvents
    @OneToMany(mappedBy = "ethernetInterface", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchPortVlan> ethernetSwitchPortVlans = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "ethernet_interface_manager",
        joinColumns = {@JoinColumn(name = "ethernet_interface_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "manager_id", referencedColumnName = "id")})
    private Set<Manager> managers = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "ethernet_interface_pcie_function",
        joinColumns = {@JoinColumn(name = "ethernet_interface_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "pcie_function_id", referencedColumnName = "id")})
    private Set<PcieDeviceFunction> pcieDeviceFunctions = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "ethernet_interface_endpoint",
        joinColumns = {@JoinColumn(name = "ethernet_interface_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "endpoint_id", referencedColumnName = "id")})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "ethernet_interface_supported_protocol", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @Column(name = "supported_protocol")
    @OrderColumn(name = "supported_protocol_order")
    private List<Protocol> supportedProtocols = new ArrayList<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public LinkStatus getLinkStatus() {
        return linkStatus;
    }

    public void setLinkStatus(LinkStatus linkStatus) {
        this.linkStatus = linkStatus;
    }

    public Boolean getInterfaceEnabled() {
        return interfaceEnabled;
    }

    public void setInterfaceEnabled(Boolean interfaceEnabled) {
        this.interfaceEnabled = interfaceEnabled;
    }

    public Boolean getAutoNeg() {
        return autoNeg;
    }

    public void setAutoNeg(Boolean autoNeg) {
        this.autoNeg = autoNeg;
    }

    public Integer getMtuSize() {
        return mtuSize;
    }

    public void setMtuSize(Integer mtuSize) {
        this.mtuSize = mtuSize;
    }

    public Integer getMaxIPv6StaticAddresses() {
        return maxIPv6StaticAddresses;
    }

    public void setMaxIPv6StaticAddresses(Integer maxIPv6StaticAddresses) {
        this.maxIPv6StaticAddresses = maxIPv6StaticAddresses;
    }

    public Boolean getVlanEnable() {
        return vlanEnable;
    }

    public void setVlanEnable(Boolean vlanEnable) {
        this.vlanEnable = vlanEnable;
    }

    public Integer getVlanId() {
        return vlanId;
    }

    public void setVlanId(Integer vlanId) {
        this.vlanId = vlanId;
    }

    public MacAddress getPermanentMacAddress() {
        return permanentMacAddress;
    }

    public void setPermanentMacAddress(MacAddress factoryMacAddress) {
        this.permanentMacAddress = factoryMacAddress;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public void setMacAddress(MacAddress macAddress) {
        this.macAddress = macAddress;
    }

    public Integer getSpeedMbps() {
        return speedMbps;
    }

    public void setSpeedMbps(Integer speedMbps) {
        this.speedMbps = speedMbps;
    }

    public Boolean isFullDuplex() {
        return fullDuplex;
    }

    public void setFullDuplex(Boolean fullDuplex) {
        this.fullDuplex = fullDuplex;
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

    public String getIpV6DefaultGateway() {
        return ipV6DefaultGateway;
    }

    public void setIpV6DefaultGateway(String ipV6DefaultGateway) {
        this.ipV6DefaultGateway = ipV6DefaultGateway;
    }

    public List<String> getNameServers() {
        return nameServers;
    }

    @Override
    public String getMultiSourceDiscriminator() {
        return multiSourceDiscriminator;
    }

    @Override
    public void setMultiSourceDiscriminator(String multiSourceDiscriminator) {
        this.multiSourceDiscriminator = multiSourceDiscriminator;
    }

    public void addNameServer(String nameServer) {
        this.nameServers.add(nameServer);
    }

    public Collection<IpV4Address> getIpV4Addresses() {
        return ipV4Addresses;
    }

    public void addIpV4Address(IpV4Address ipV4Address) {
        ipV4Addresses.add(ipV4Address);
    }

    public List<IpV6Address> getIpV6Addresses() {
        return ipV6Addresses;
    }

    public void addIpV6Address(IpV6Address ipV6Address) {
        ipV6Addresses.add(ipV6Address);
    }

    public List<IpV6Address> getIpV6StaticAddresses() {
        return ipV6StaticAddresses;
    }

    public void addIpV6StaticAddress(IpV6Address ipV6Address) {
        ipV6StaticAddresses.add(ipV6Address);
    }

    public List<IpV6AddressPolicy> getIpV6AddressPolicyTable() {
        return ipV6AddressPolicyTable;
    }

    public void addIpV6Policy(IpV6AddressPolicy ipV6AddressPolicy) {
        ipV6AddressPolicyTable.add(ipV6AddressPolicy);
    }

    public List<Protocol> getSupportedProtocols() {
        return supportedProtocols;
    }
    public void addSupportedProtocol(Protocol supportedProtocol) {
        supportedProtocols.add(supportedProtocol);
    }

    @Override
    public Set<EthernetSwitchPortVlan> getEthernetSwitchPortVlans() {
        return ethernetSwitchPortVlans;
    }

    public void addEthernetSwitchPortVlan(EthernetSwitchPortVlan ethernetSwitchPortVlan) {
        requiresNonNull(ethernetSwitchPortVlan, "ethernetSwitchPortVlan");

        this.ethernetSwitchPortVlans.add(ethernetSwitchPortVlan);
        if (!this.equals(ethernetSwitchPortVlan.getEthernetInterface())) {
            ethernetSwitchPortVlan.setEthernetInterface(this);
        }
    }

    public void unlinkEthernetSwitchPortVlan(EthernetSwitchPortVlan ethernetSwitchPortVlan) {
        if (this.ethernetSwitchPortVlans.contains(ethernetSwitchPortVlan)) {
            this.ethernetSwitchPortVlans.remove(ethernetSwitchPortVlan);
            if (ethernetSwitchPortVlan != null) {
                ethernetSwitchPortVlan.unlinkEthernetInterface(this);
            }
        }
    }

    public Set<Manager> getManagers() {
        return managers;
    }

    public void addManager(Manager manager) {
        requiresNonNull(manager, "manager");

        managers.add(manager);
        if (!manager.getEthernetInterfaces().contains(this)) {
            manager.addEthernetInterface(this);
        }
    }

    public void unlinkManager(Manager manager) {
        if (managers.contains(manager)) {
            managers.remove(manager);
            if (manager != null) {
                manager.unlinkEthernetInterface(this);
            }
        }
    }

    public Set<PcieDeviceFunction> getPcieDeviceFunctions() {
        return pcieDeviceFunctions;
    }

    public void addPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        requiresNonNull(pcieDeviceFunction, "pcieDeviceFunction");

        pcieDeviceFunctions.add(pcieDeviceFunction);
        if (!pcieDeviceFunction.getEthernetInterfaces().contains(this)) {
            pcieDeviceFunction.addEthernetInterface(this);
        }
    }

    public void unlinkPcieDeviceFunction(PcieDeviceFunction pcieDeviceFunction) {
        if (pcieDeviceFunctions.contains(pcieDeviceFunction)) {
            pcieDeviceFunctions.remove(pcieDeviceFunction);
            if (pcieDeviceFunction != null) {
                pcieDeviceFunction.unlinkEthernetInterface(this);
            }
        }
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!endpoint.getEthernetInterfaces().contains(this)) {
            endpoint.addEthernetInterface(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkEthernetInterface(this);
            }
        }
    }

    public ComputerSystem getComputerSystem() {
        return computerSystem;
    }

    public void setComputerSystem(ComputerSystem computerSystem) {
        if (!Objects.equals(this.computerSystem, computerSystem)) {
            unlinkComputerSystem(this.computerSystem);
            this.computerSystem = computerSystem;
            if (computerSystem != null && !computerSystem.getEthernetInterfaces().contains(this)) {
                computerSystem.addEthernetInterface(this);
            }
        }
    }

    public void unlinkComputerSystem(ComputerSystem computerSystem) {
        if (Objects.equals(this.computerSystem, computerSystem)) {
            this.computerSystem = null;
            if (computerSystem != null) {
                computerSystem.unlinkEthernetInterface(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(ethernetSwitchPortVlans, this::unlinkEthernetSwitchPortVlan);
        unlinkCollection(managers, this::unlinkManager);
        unlinkCollection(pcieDeviceFunctions, this::unlinkPcieDeviceFunction);
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkComputerSystem(computerSystem);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, managers) || isContainedBy(possibleParent, computerSystem);
    }
}
