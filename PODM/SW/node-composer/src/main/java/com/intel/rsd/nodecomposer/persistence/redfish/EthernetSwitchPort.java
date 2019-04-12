/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish;

import com.intel.rsd.nodecomposer.persistence.IgnoreUnlinkingRelationship;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import lombok.Getter;
import lombok.Setter;
import org.hibernate.annotations.NotFound;

import javax.persistence.Column;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.NotFoundAction.IGNORE;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = EthernetSwitchPort.GET_ETHERNET_SWITCH_PORT_BY_NEIGHBOR_MAC,
        query = "SELECT esp FROM EthernetSwitchPort esp WHERE esp.neighborMac = :neighborMac")
})
@Table(name = "ethernet_switch_port")
public class EthernetSwitchPort extends DiscoverableEntity {
    public static final String GET_ETHERNET_SWITCH_PORT_BY_NEIGHBOR_MAC = "GET_ETHERNET_SWITCH_PORT_BY_NEIGHBOR_MAC";

    @Getter
    @Setter
    @Column(name = "neighbor_mac")
    private MacAddress neighborMac;

    @OneToMany(mappedBy = "ethernetSwitchPort", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchPortVlan> ethernetSwitchPortVlans = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "primary_vlan_id")
    @NotFound(action = IGNORE)
    private EthernetSwitchPortVlan primaryVlan;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_id")
    private EthernetSwitch ethernetSwitch;

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

    @Override
    public void preRemove() {
        unlinkCollection(ethernetSwitchPortVlans, this::unlinkEthernetSwitchPortVlan);
        unlinkPrimaryVlan(primaryVlan);
        unlinkEthernetSwitch(ethernetSwitch);
    }
}
