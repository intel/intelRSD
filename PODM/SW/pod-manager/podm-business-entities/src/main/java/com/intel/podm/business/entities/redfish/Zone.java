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
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "zone", indexes = @Index(name = "idx_zone_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class Zone extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @OneToMany(mappedBy = "zone", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToMany(mappedBy = "zones", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Switch> switches = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public Set<Endpoint> getEndpoints() {
        return endpoints;
    }

    public void addEndpoint(Endpoint endpoint) {
        requiresNonNull(endpoint, "endpoint");
        endpoints.add(endpoint);
        if (!this.equals(endpoint.getZone())) {
            endpoint.setZone(this);
        }
    }

    public void unlinkEndpoint(Endpoint endpointToUnlink) {
        if (endpoints.contains(endpointToUnlink)) {
            endpoints.remove(endpointToUnlink);
            if (endpointToUnlink != null) {
                endpointToUnlink.unlinkZone(this);
            }
        }
    }

    public Set<Switch> getSwitches() {
        return switches;
    }

    public void addSwitch(Switch fabricSwitch) {
        requiresNonNull(fabricSwitch, "fabricSwitch");

        switches.add(fabricSwitch);
        if (!fabricSwitch.getZones().contains(this)) {
            fabricSwitch.addZone(this);
            if (fabric != null) {
                fabricSwitch.setFabric(fabric);
            }
        }
    }

    public void unlinkSwitch(Switch switchToUnlink) {
        if (switches.contains(switchToUnlink)) {
            switches.remove(switchToUnlink);
            if (switchToUnlink != null) {
                switchToUnlink.setFabric(null);
                switchToUnlink.unlinkZone(this);
            }
        }
    }

    public Fabric getFabric() {
        return fabric;
    }

    public void setFabric(Fabric fabric) {
        if (fabric == null) {
            unlinkFabric(this.fabric);
        } else {
            this.fabric = fabric;
            if (!fabric.getZones().contains(this)) {
                fabric.addZone(this);
            }
        }
    }

    public void unlinkFabric(Fabric fabric) {
        if (Objects.equals(this.fabric, fabric)) {
            this.fabric = null;
            if (fabric != null) {
                fabric.unlinkZone(this);
            }
        }
    }

    public void clearEndpointsCollection() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
    }

    @Override
    public void preRemove() {
        unlinkCollection(endpoints, this::unlinkEndpoint);
        unlinkCollection(switches, this::unlinkSwitch);
        unlinkFabric(fabric);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, fabric);
    }
}
