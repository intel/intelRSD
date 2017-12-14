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
import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.base.VlanPossessor;
import com.intel.podm.business.entities.redfish.embeddables.IpV4Address;
import com.intel.podm.business.entities.redfish.embeddables.IpV6Address;
import com.intel.podm.common.types.AdministrativeState;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.LinkType;
import com.intel.podm.common.types.NeighborInfo;
import com.intel.podm.common.types.OperationalState;
import com.intel.podm.common.types.PortClass;
import com.intel.podm.common.types.PortMode;
import com.intel.podm.common.types.PortType;
import com.intel.podm.common.types.net.MacAddress;
import org.hibernate.annotations.NotFound;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.NotFoundAction.IGNORE;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = EthernetSwitchPort.GET_PORT_BY_NEIGHBOR_MAC_AND_PORT_TYPE,
        query = "SELECT esp "
            + "FROM EthernetSwitchPort esp "
            + "WHERE esp.neighborMac = :neighborMac "
            + "AND esp.portType = :portType")
})
@Table(name = "ethernet_switch_port", indexes = @Index(name = "idx_ethernet_switch_port_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:ClassFanOutComplexity", "checkstyle:MethodCount"})
public class EthernetSwitchPort extends DiscoverableEntity implements VlanPossessor {
    public static final String GET_PORT_BY_NEIGHBOR_MAC_AND_PORT_TYPE = "GET_PORT_BY_NEIGHBOR_MAC_AND_PORT_TYPE";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "port_id")
    private String portId;

    @Column(name = "link_type")
    @Enumerated(STRING)
    private LinkType linkType;

    @Column(name = "operational_state")
    @Enumerated(STRING)
    private OperationalState operationalState;

    @Column(name = "administrative_state")
    @Enumerated(STRING)
    private AdministrativeState administrativeState;

    @Column(name = "link_speed_gbps")
    private Integer linkSpeedGbps;

    @Column(name = "neighbor_info")
    private NeighborInfo neighborInfo;

    @Column(name = "neighbor_mac")
    private MacAddress neighborMac;

    @Column(name = "mac_address")
    private MacAddress macAddress;

    @Column(name = "autosense")
    private Boolean autosense;

    @Column(name = "frame_size")
    private Integer frameSize;

    @Column(name = "full_duplex")
    private Boolean fullDuplex;

    @Column(name = "port_class")
    @Enumerated(STRING)
    private PortClass portClass;

    @Column(name = "port_mode")
    @Enumerated(STRING)
    private PortMode portMode;

    @Column(name = "port_type")
    @Enumerated(STRING)
    private PortType portType;

    @ElementCollection
    @CollectionTable(name = "ethernet_switch_port_ipv4_address", joinColumns = @JoinColumn(name = "ethernet_switch_port_id"))
    @OrderColumn(name = "ipv4_address_order")
    private List<IpV4Address> ipv4Addresses = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "ethernet_switch_port_ipv6_address", joinColumns = @JoinColumn(name = "ethernet_switch_port_id"))
    @OrderColumn(name = "ipv6_address_order")
    private List<IpV6Address> ipv6Addresses = new ArrayList<>();

    @OneToMany(mappedBy = "memberOfPort", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchPort> portMembers = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "ethernetSwitchPort", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchPortVlan> ethernetSwitchPortVlans = new HashSet<>();

    @OneToMany(mappedBy = "ethernetSwitchPort", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchStaticMac> ethernetSwitchStaticMacs = new HashSet<>();

    @ManyToMany(mappedBy = "boundPorts", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchAcl> activeAcls = new HashSet<>();

    @ManyToMany(mappedBy = "bindActionAllowableValues", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchAcl> aclsToBind = new HashSet<>();

    @SuppressEvents
    @ManyToMany(mappedBy = "mirrorPortRegions", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchAclRule> mirrorPortRegionForRules = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "primary_vlan_id")
    @NotFound(action = IGNORE)
    private EthernetSwitchPortVlan primaryVlan;

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_port_metrics_id")
    private EthernetSwitchPortMetrics metrics;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_id")
    private EthernetSwitch ethernetSwitch;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "member_of_port_id")
    private EthernetSwitchPort memberOfPort;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getPortId() {
        return portId;
    }

    public void setPortId(String portId) {
        this.portId = portId;
    }

    public LinkType getLinkType() {
        return linkType;
    }

    public void setLinkType(LinkType value) {
        this.linkType = value;
    }

    public OperationalState getOperationalState() {
        return operationalState;
    }

    public void setOperationalState(OperationalState value) {
        this.operationalState = value;
    }

    public AdministrativeState getAdministrativeState() {
        return administrativeState;
    }

    public void setAdministrativeState(AdministrativeState value) {
        this.administrativeState = value;
    }

    public Integer getLinkSpeedMbps() {
        return linkSpeedGbps;
    }

    public void setLinkSpeedMbps(Integer value) {
        this.linkSpeedGbps = value;
    }

    public NeighborInfo getNeighborInfo() {
        return neighborInfo;
    }

    public void setNeighborInfo(NeighborInfo neighborInfo) {
        this.neighborInfo = neighborInfo;
    }

    public MacAddress getNeighborMac() {
        return neighborMac;
    }

    public void setNeighborMac(MacAddress macAddress) {
        this.neighborMac = macAddress;
    }

    public MacAddress getMacAddress() {
        return macAddress;
    }

    public void setMacAddress(MacAddress macAddress) {
        this.macAddress = macAddress;
    }

    public Boolean getAutosense() {
        return autosense;
    }

    public void setAutosense(Boolean autosense) {
        this.autosense = autosense;
    }

    public Integer getFrameSize() {
        return frameSize;
    }

    public void setFrameSize(Integer frameSize) {
        this.frameSize = frameSize;
    }

    public Boolean getFullDuplex() {
        return fullDuplex;
    }

    public void setFullDuplex(Boolean fullDuplex) {
        this.fullDuplex = fullDuplex;
    }

    public PortClass getPortClass() {
        return portClass;
    }

    public void setPortClass(PortClass portClass) {
        this.portClass = portClass;
    }

    public PortMode getPortMode() {
        return portMode;
    }

    public void setPortMode(PortMode portMode) {
        this.portMode = portMode;
    }

    public PortType getPortType() {
        return portType;
    }

    public void setPortType(PortType portType) {
        this.portType = portType;
    }

    public Collection<IpV4Address> getIpV4Addresses() {
        return ipv4Addresses;
    }

    public void addIpV4Address(IpV4Address address) {
        this.ipv4Addresses.add(address);
    }

    public Collection<IpV6Address> getIpV6Addresses() {
        return ipv6Addresses;
    }

    public void addIpV6Address(IpV6Address address) {
        this.ipv6Addresses.add(address);
    }

    public Set<EthernetSwitchPort> getPortMembers() {
        return portMembers;
    }

    public void addPortMember(EthernetSwitchPort ethernetSwitchPort) {
        requiresNonNull(ethernetSwitchPort, "ethernetSwitchPort");

        portMembers.add(ethernetSwitchPort);
        if (!this.equals(ethernetSwitchPort.getMemberOfPort())) {
            ethernetSwitchPort.setMemberOfPort(this);
        }
    }

    public void unlinkPortMember(EthernetSwitchPort ethernetSwitchPort) {
        if (portMembers.contains(ethernetSwitchPort)) {
            portMembers.remove(ethernetSwitchPort);
            if (ethernetSwitchPort != null) {
                ethernetSwitchPort.unlinkMemberOfPort(this);
            }
        }
    }

    public void uncouplePortMembers(Predicate<EthernetSwitchPort> unlinkPredicate) {
        unlinkCollection(portMembers, this::unlinkPortMember, unlinkPredicate);
    }

    @Override
    public Set<EthernetSwitchPortVlan> getEthernetSwitchPortVlans() {
        return ethernetSwitchPortVlans;
    }

    public void addEthernetSwitchPortVlan(EthernetSwitchPortVlan ethernetSwitchPortVlan) {
        requiresNonNull(ethernetSwitchPortVlan, "ethernetSwitchPortVlan");

        ethernetSwitchPortVlans.add(ethernetSwitchPortVlan);
        if (!this.equals(ethernetSwitchPortVlan.getEthernetSwitchPort())) {
            ethernetSwitchPortVlan.setEthernetSwitchPort(this);
        }
    }

    public void unlinkEthernetSwitchPortVlan(EthernetSwitchPortVlan ethernetSwitchPortVlan) {
        if (ethernetSwitchPortVlans.contains(ethernetSwitchPortVlan)) {
            ethernetSwitchPortVlans.remove(ethernetSwitchPortVlan);
            if (ethernetSwitchPortVlan != null) {
                ethernetSwitchPortVlan.unlinkEthernetSwitchPort(this);
            }
        }
    }

    public Set<EthernetSwitchStaticMac> getEthernetSwitchStaticMacs() {
        return ethernetSwitchStaticMacs;
    }

    public void addEthernetSwitchStaticMac(EthernetSwitchStaticMac ethernetSwitchStaticMac) {
        requiresNonNull(ethernetSwitchStaticMac, "ethernetSwitchStaticMac");

        ethernetSwitchStaticMacs.add(ethernetSwitchStaticMac);
        if (!this.equals(ethernetSwitchStaticMac.getEthernetSwitchPort())) {
            ethernetSwitchStaticMac.setEthernetSwitchPort(this);
        }
    }

    public void unlinkEthernetSwitchStaticMac(EthernetSwitchStaticMac ethernetSwitchStaticMac) {
        if (ethernetSwitchStaticMacs.contains(ethernetSwitchStaticMac)) {
            ethernetSwitchStaticMacs.remove(ethernetSwitchStaticMac);
            if (ethernetSwitchStaticMac != null) {
                ethernetSwitchStaticMac.unlinkEthernetSwitchPort(this);
            }
        }
    }

    public Set<EthernetSwitchAcl> getActiveAcls() {
        return activeAcls;
    }

    public void addActiveAcl(EthernetSwitchAcl ethernetSwitchAcl) {
        requiresNonNull(ethernetSwitchAcl, "ethernetSwitchAcl");

        activeAcls.add(ethernetSwitchAcl);
        if (!ethernetSwitchAcl.getBoundPorts().contains(this)) {
            ethernetSwitchAcl.addBoundPort(this);
        }
    }

    public void unlinkActiveAcl(EthernetSwitchAcl ethernetSwitchAcl) {
        if (activeAcls.contains(ethernetSwitchAcl)) {
            activeAcls.remove(ethernetSwitchAcl);
            if (ethernetSwitchAcl != null) {
                ethernetSwitchAcl.unlinkBoundPort(this);
            }
        }
    }

    public Set<EthernetSwitchAcl> getAclsToBind() {
        return aclsToBind;
    }

    public void addAclsToBind(EthernetSwitchAcl ethernetSwitchAcl) {
        requiresNonNull(ethernetSwitchAcl, "ethernetSwitchAcl");

        aclsToBind.add(ethernetSwitchAcl);
        if (!ethernetSwitchAcl.getBindActionAllowableValues().contains(this)) {
            ethernetSwitchAcl.addBindActionAllowableValue(this);
        }
    }

    public void unlinkAclsToBind(EthernetSwitchAcl ethernetSwitchAcl) {
        if (aclsToBind.contains(ethernetSwitchAcl)) {
            aclsToBind.remove(ethernetSwitchAcl);
            if (ethernetSwitchAcl != null) {
                ethernetSwitchAcl.unlinkBindActionAllowableValue(this);
            }
        }
    }

    public Set<EthernetSwitchAclRule> getMirrorPortRegionForRules() {
        return mirrorPortRegionForRules;
    }

    public void addMirrorPortRegionForRules(EthernetSwitchAclRule ethernetSwitchAclRule) {
        requiresNonNull(ethernetSwitchAclRule, "ethernetSwitchAclRule");

        mirrorPortRegionForRules.add(ethernetSwitchAclRule);
        if (!ethernetSwitchAclRule.getMirrorPortRegions().contains(this)) {
            ethernetSwitchAclRule.addMirrorPort(this);
        }
    }

    public void unlinkMirrorPortRegionForRules(EthernetSwitchAclRule ethernetSwitchAclRule) {
        if (mirrorPortRegionForRules.contains(ethernetSwitchAclRule)) {
            mirrorPortRegionForRules.remove(ethernetSwitchAclRule);
            if (ethernetSwitchAclRule != null) {
                ethernetSwitchAclRule.unlinkMirrorPortRegion(this);
            }
        }
    }

    public EthernetSwitchPortVlan getPrimaryVlan() {
        return primaryVlan;
    }

    public void setPrimaryVlan(EthernetSwitchPortVlan primaryVlan) {
        if (!Objects.equals(this.primaryVlan, primaryVlan)) {
            unlinkPrimaryVlan(this.primaryVlan);
            this.primaryVlan = primaryVlan;
        }
    }

    public void unlinkPrimaryVlan(EthernetSwitchPortVlan primaryVlan) {
        if (Objects.equals(this.primaryVlan, primaryVlan)) {
            this.primaryVlan = null;
        }
    }

    public EthernetSwitchPortMetrics getMetrics() {
        return metrics;
    }

    public void setMetrics(EthernetSwitchPortMetrics metrics) {
        if (!Objects.equals(this.metrics, metrics)) {
            unlinkMetrics(this.metrics);
            this.metrics = metrics;
            if (metrics != null && !this.equals(metrics.getEthernetSwitchPort())) {
                metrics.setEthernetSwitchPort(this);
            }
        }
    }

    public void unlinkMetrics(EthernetSwitchPortMetrics metrics) {
        if (Objects.equals(this.metrics, metrics)) {
            this.metrics = null;
            if (metrics != null) {
                metrics.unlinkEthernetSwitchPort(this);
            }
        }
    }

    public EthernetSwitch getEthernetSwitch() {
        return ethernetSwitch;
    }

    public void setEthernetSwitch(EthernetSwitch ethernetSwitch) {
        if (!Objects.equals(this.ethernetSwitch, ethernetSwitch)) {
            unlinkEthernetSwitch(this.ethernetSwitch);
            this.ethernetSwitch = ethernetSwitch;
            if (ethernetSwitch != null && !ethernetSwitch.getPorts().contains(this)) {
                ethernetSwitch.addPort(this);
            }
        }
    }

    public void unlinkEthernetSwitch(EthernetSwitch ethernetSwitch) {
        if (Objects.equals(this.ethernetSwitch, ethernetSwitch)) {
            this.ethernetSwitch = null;
            if (ethernetSwitch != null) {
                ethernetSwitch.unlinkPort(this);
            }
        }
    }

    public EthernetSwitchPort getMemberOfPort() {
        return memberOfPort;
    }

    public void setMemberOfPort(EthernetSwitchPort memberOfPort) {
        if (!Objects.equals(this.memberOfPort, memberOfPort)) {
            unlinkMemberOfPort(this.memberOfPort);
            this.memberOfPort = memberOfPort;
            if (memberOfPort != null && !memberOfPort.getPortMembers().contains(this)) {
                memberOfPort.addPortMember(this);
            }
        }
    }

    public void unlinkMemberOfPort(EthernetSwitchPort memberOfPort) {
        if (Objects.equals(this.memberOfPort, memberOfPort)) {
            this.memberOfPort = null;
            if (memberOfPort != null) {
                memberOfPort.unlinkPortMember(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(portMembers, this::unlinkPortMember);
        unlinkCollection(ethernetSwitchPortVlans, this::unlinkEthernetSwitchPortVlan);
        unlinkCollection(ethernetSwitchStaticMacs, this::unlinkEthernetSwitchStaticMac);
        unlinkCollection(activeAcls, this::unlinkActiveAcl);
        unlinkCollection(aclsToBind, this::unlinkAclsToBind);
        unlinkCollection(mirrorPortRegionForRules, this::unlinkMirrorPortRegionForRules);
        unlinkPrimaryVlan(primaryVlan);
        unlinkMetrics(metrics);
        unlinkEthernetSwitch(ethernetSwitch);
        unlinkMemberOfPort(memberOfPort);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, ethernetSwitch);
    }
}
