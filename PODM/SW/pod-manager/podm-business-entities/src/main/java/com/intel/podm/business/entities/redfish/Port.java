/*
 * Copyright (c) 2016-2018 Intel Corporation
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
import com.intel.podm.business.entities.redfish.base.Resettable;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.PciePortType;
import com.intel.podm.common.types.Protocol;
import com.intel.podm.common.types.actions.ResetType;

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
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.Port.GET_PORTS_BY_PCIE_CONNECTION_ID;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "port", indexes = @Index(name = "idx_port_entity_id", columnList = "entity_id", unique = true))
@NamedQueries({
    @NamedQuery(name = GET_PORTS_BY_PCIE_CONNECTION_ID,
        query = "SELECT port FROM Port port WHERE :pcieConnectionId MEMBER OF port.pcieConnectionIds")
})
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class Port extends DiscoverableEntity implements Resettable {
    public static final String GET_PORTS_BY_PCIE_CONNECTION_ID = "GET_PORTS_BY_PCIE_CONNECTION_ID";
    private static final String ENTITY_NAME = "Port";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "port_id")
    private String portId;

    @Column(name = "port_type")
    @Enumerated(STRING)
    private PciePortType portType;

    @Column(name = "port_protocol")
    @Enumerated(STRING)
    private Protocol portProtocol;

    @Column(name = "current_speed_gbps")
    private Long currentSpeedGbps;

    @Column(name = "max_speed_gbps")
    private Long maxSpeedGbps;

    @Column(name = "width")
    private Long width;

    @ElementCollection
    @CollectionTable(name = "port_pcie_connection_id", joinColumns = @JoinColumn(name = "port_id"))
    @Column(name = "pcie_connection_id")
    @OrderColumn(name = "pcie_connection_id_order")
    private List<String> pcieConnectionIds = new ArrayList<>();

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "port_allowable_reset_type", joinColumns = @JoinColumn(name = "port_id"))
    @Column(name = "allowable_reset_type")
    @OrderColumn(name = "allowable_reset_type_order")
    private List<ResetType> allowableResetTypes = new ArrayList<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "port_endpoint",
        joinColumns = {@JoinColumn(name = "port_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "endpoint_id", referencedColumnName = "id")})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_switch_id")
    private Switch fabricSwitch;

    @OneToOne(mappedBy = "port", fetch = LAZY, cascade = {MERGE, PERSIST})
    private PortMetrics portMetrics;

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

    public PciePortType getPortType() {
        return portType;
    }

    public void setPortType(PciePortType portType) {
        this.portType = portType;
    }

    public Protocol getPortProtocol() {
        return portProtocol;
    }

    public void setPortProtocol(Protocol portProtocol) {
        this.portProtocol = portProtocol;
    }

    public Long getCurrentSpeedGbps() {
        return currentSpeedGbps;
    }

    public void setCurrentSpeedGbps(Long speedGbps) {
        this.currentSpeedGbps = speedGbps;
    }

    public Long getMaxSpeedGbps() {
        return maxSpeedGbps;
    }

    public void setMaxSpeedGbps(Long maxSpeedGbps) {
        this.maxSpeedGbps = maxSpeedGbps;
    }

    public Long getWidth() {
        return width;
    }

    public void setWidth(Long width) {
        this.width = width;
    }

    public List<String> getPcieConnectionIds() {
        return pcieConnectionIds;
    }

    public void addPcieConnectionId(String pcieConnectionId) {
        this.pcieConnectionIds.add(pcieConnectionId);
    }

    @Override
    public List<ResetType> getAllowableResetTypes() {
        return allowableResetTypes;
    }

    public void addAllowableResetType(ResetType allowableResetType) {
        allowableResetTypes.add(allowableResetType);
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

    public PortMetrics getPortMetrics() {
        return portMetrics;
    }

    public void setPortMetrics(PortMetrics portMetrics) {
        if (!Objects.equals(this.portMetrics, portMetrics)) {
            unlinkMetrics(this.portMetrics);
            this.portMetrics = portMetrics;
            if (portMetrics != null && !this.equals(portMetrics.getPort())) {
                portMetrics.setPort(this);
            }
        }
    }

    public void unlinkMetrics(PortMetrics portMetrics) {
        if (Objects.equals(this.portMetrics, portMetrics)) {
            this.portMetrics = null;
            if (portMetrics != null) {
                portMetrics.unlinkPort(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkSwitch(fabricSwitch);
        unlinkMetrics(portMetrics);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, fabricSwitch);
    }

    @Override
    public String getResetName() {
        return ENTITY_NAME;
    }
}
