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

import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "zone")
public class Zone extends DiscoverableEntity {
    @OneToMany(mappedBy = "zone", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Endpoint> endpoints = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "fabric_id")
    private Fabric fabric;

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
        unlinkFabric(fabric);
    }
}
