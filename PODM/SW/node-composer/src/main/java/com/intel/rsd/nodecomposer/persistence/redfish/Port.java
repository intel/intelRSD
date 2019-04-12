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

import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.types.PciePortType;
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
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.Port.GET_PORTS_BY_PCIE_CONNECTION_ID;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "port")
@NamedQueries({
    @NamedQuery(name = GET_PORTS_BY_PCIE_CONNECTION_ID,
        query = "SELECT port FROM Port port WHERE :pcieConnectionId MEMBER OF port.pcieConnectionIds")
})
public class Port extends DiscoverableEntity {
    public static final String GET_PORTS_BY_PCIE_CONNECTION_ID = "GET_PORTS_BY_PCIE_CONNECTION_ID";

    @Getter
    @Setter
    @Column(name = "port_type")
    @Enumerated(STRING)
    private PciePortType portType;

    @ElementCollection
    @CollectionTable(name = "port_pcie_connection_id", joinColumns = @JoinColumn(name = "port_id"))
    @Column(name = "pcie_connection_id")
    @OrderColumn(name = "pcie_connection_id_order")
    private List<String> pcieConnectionIds = new ArrayList<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "port_endpoint",
        joinColumns = {@JoinColumn(name = "port_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "endpoint_id", referencedColumnName = "id")})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_switch_id")
    private Switch fabricSwitch;

    public List<String> getPcieConnectionIds() {
        return pcieConnectionIds;
    }

    public void addPcieConnectionId(String pcieConnectionId) {
        this.pcieConnectionIds.add(pcieConnectionId);
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");

        endpoints.add(endpoint);
        if (!endpoint.getPorts().contains(this)) {
            endpoint.addPort(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkPort(this);
            }
        }
    }

    public Switch getSwitch() {
        return fabricSwitch;
    }

    public void setSwitch(Switch fabricSwitch) {
        if (fabricSwitch == null) {
            unlinkSwitch(this.fabricSwitch);
        } else {
            this.fabricSwitch = fabricSwitch;
            if (!fabricSwitch.getPorts().contains(this)) {
                fabricSwitch.addPort(this);
            }
        }
    }

    public void unlinkSwitch(Switch fabricSwitch) {
        if (Objects.equals(this.fabricSwitch, fabricSwitch)) {
            this.fabricSwitch = null;
            if (fabricSwitch != null) {
                fabricSwitch.unlinkPort(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkSwitch(fabricSwitch);
    }
}
