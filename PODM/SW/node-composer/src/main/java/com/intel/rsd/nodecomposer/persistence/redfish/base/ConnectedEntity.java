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

package com.intel.rsd.nodecomposer.persistence.redfish.base;

import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.embeddables.Identifier;
import com.intel.rsd.nodecomposer.types.EntityRole;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.EAGER;

@javax.persistence.Entity
@Table(name = "connected_entity")
public class ConnectedEntity extends DiscoverableEntity {
    @Getter
    @Setter
    @Column(name = "entity_role")
    @Enumerated(STRING)
    private EntityRole entityRole;

    @ElementCollection
    @CollectionTable(name = "connected_entity_identifier", joinColumns = @JoinColumn(name = "connected_entity_id"))
    @OrderColumn(name = "connected_entity_identifier_order")
    private Set<Identifier> identifiers = new HashSet<>();

    @ManyToOne(fetch = EAGER, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "discoverable_entity_id")
    private DiscoverableEntity entityLink;

    @ManyToOne(fetch = EAGER, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "endpoint_id")
    private Endpoint endpoint;

    @Getter
    @Setter
    @Column(name = "lun")
    private Integer lun;

    public Set<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public DiscoverableEntity getEntityLink() {
        return entityLink;
    }

    public void setEntityLink(DiscoverableEntity entity) {
        if (!Objects.equals(this.entityLink, entity)) {
            unlinkEntityLink(this.entityLink);
            this.entityLink = entity;
            if (entity != null && !entity.getEntityConnections().contains(this)) {
                entity.addEntityConnection(this);
            }
        }
    }

    public void unlinkEntityLink(DiscoverableEntity entity) {
        if (Objects.equals(this.entityLink, entity)) {
            this.entityLink = null;
            if (entity != null) {
                entity.unlinkEntityConnection(this);
            }
        }
    }

    public Endpoint getEndpoint() {
        return endpoint;
    }

    public void setEndpoint(Endpoint endpoint) {
        if (endpoint == null) {
            unlinkEndpoint(this.endpoint);
        } else {
            this.endpoint = endpoint;
            if (!endpoint.getConnectedEntities().contains(this)) {
                endpoint.addConnectedEntity(this);
            }
        }
    }

    public void unlinkEndpoint(Endpoint endpoint) {
        if (Objects.equals(this.endpoint, endpoint)) {
            this.endpoint = null;
            if (endpoint != null) {
                endpoint.unlinkConnectedEntities(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkEntityLink(entityLink);
        unlinkEndpoint(endpoint);
    }
}
