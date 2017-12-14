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
import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.Protocol;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = Fabric.GET_ALL_FABRIC_IDS, query = "SELECT fabric.entityId FROM Fabric fabric")
})
@Table(name = "fabric", indexes = @Index(name = "idx_fabric_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class Fabric extends DiscoverableEntity {
    public static final String GET_ALL_FABRIC_IDS = "GET_ALL_FABRIC_IDS";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "max_zones")
    private Integer maxZones;

    @Column(name = "fabric_type")
    @Enumerated(STRING)
    private Protocol fabricType;

    @SuppressEvents
    @OneToMany(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Zone> zones = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Switch> switches = new HashSet<>();

    @SuppressEvents
    @OneToMany(mappedBy = "fabric", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public Integer getMaxZones() {
        return maxZones;
    }

    public void setMaxZones(Integer maxZones) {
        this.maxZones = maxZones;
    }

    public Protocol getFabricType() {
        return fabricType;
    }

    public void setFabricType(Protocol fabricType) {
        this.fabricType = fabricType;
    }

    public Set<Zone> getZones() {
        return zones;
    }

    public void addZone(Zone zone) {
        requiresNonNull(zone, "zone");
        zones.add(zone);
        if (!this.equals(zone.getFabric())) {
            zone.setFabric(this);
        }
    }

    public void unlinkZone(Zone zone) {
        if (zones.contains(zone)) {
            zones.remove(zone);
            if (zone != null) {
                zone.unlinkFabric(this);
            }
        }
    }

    public Set<Switch> getSwitches() {
        return switches;
    }

    public void addSwitch(Switch fabricSwitch) {
        requiresNonNull(fabricSwitch, "fabricSwitch");
        switches.add(fabricSwitch);
        if (!this.equals(fabricSwitch.getFabric())) {
            fabricSwitch.setFabric(this);
        }
    }

    public void unlinkSwitch(Switch fabricSwitch) {
        if (switches.contains(fabricSwitch)) {
            switches.remove(fabricSwitch);
            if (fabricSwitch != null) {
                fabricSwitch.unlinkFabric(this);
            }
        }
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");
        endpoints.add(endpoint);
        if (!this.equals(endpoint.getFabric())) {
            endpoint.setFabric(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (endpoints.contains(endpoint)) {
            endpoints.remove(endpoint);
            if (endpoint != null) {
                endpoint.unlinkFabric(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(zones, this::unlinkZone);
        unlinkCollection(switches, this::unlinkSwitch);
        unlinkCollection(endpoints, this::unlinkEndpoint);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
