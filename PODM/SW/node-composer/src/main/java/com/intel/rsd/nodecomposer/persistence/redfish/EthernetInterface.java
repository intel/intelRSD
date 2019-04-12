/*
 * Copyright (c) 2015-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.Protocol;
import com.intel.rsd.nodecomposer.types.net.MacAddress;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "ethernet_interface")
public class EthernetInterface extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "mac_address")
    private MacAddress macAddress;

    @Getter
    @Setter
    @Column(name = "speed_mbps")
    private Integer speedMbps;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "ethernet_interface_supported_protocol", joinColumns = @JoinColumn(name = "ethernet_interface_id"))
    @Column(name = "supported_protocol")
    @OrderColumn(name = "supported_protocol_order")
    private List<Protocol> supportedProtocols = new ArrayList<>();

    @OneToMany(mappedBy = "ethernetInterface", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<EthernetSwitchPortVlan> ethernetSwitchPortVlans = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "ethernet_interface_endpoint",
        joinColumns = {@JoinColumn(name = "ethernet_interface_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "endpoint_id", referencedColumnName = "id")})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "computer_system_id")
    private ComputerSystem computerSystem;

    public List<Protocol> getSupportedProtocols() {
        return supportedProtocols;
    }

    public void addSupportedProtocol(Protocol supportedProtocol) {
        supportedProtocols.add(supportedProtocol);
    }

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
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkComputerSystem(computerSystem);
    }
}
