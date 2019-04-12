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

package com.intel.rsd.nodecomposer.persistence.redfish.base;

import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.types.Status;
import lombok.Getter;
import lombok.Setter;

import javax.persistence.Column;
import javax.persistence.DiscriminatorColumn;
import javax.persistence.Inheritance;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.PreRemove;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Set;

import static com.intel.rsd.nodecomposer.persistence.redfish.base.StatusControl.statusOf;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;
import static javax.persistence.InheritanceType.JOINED;

@javax.persistence.Entity
@Inheritance(strategy = JOINED)
@DiscriminatorColumn(name = "discriminator_class", length = 100)
@Table(name = "discoverable_entity")
@NamedQueries({
    @NamedQuery(name = DiscoverableEntity.GET_DISCOVERABLE_ENTITY_BY_URI,
        query = "SELECT es FROM DiscoverableEntity es WHERE es.uri = :uri"),
})
public abstract class DiscoverableEntity extends Entity {
    public static final String GET_DISCOVERABLE_ENTITY_BY_URI = "GET_DISCOVERABLE_ENTITY_BY_URI";

    @Getter
    @Setter
    @Column(name = "name")
    private String name;

    @Getter
    @Setter
    @Column(name = "description")
    private String description;

    @Getter
    @Setter
    @Column(name = "status")
    private Status status;

    @Getter
    @Setter
    @Column(name = "uri", unique = true)
    private ODataId uri;

    @OneToMany(mappedBy = "entityLink", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ConnectedEntity> entityConnections = new HashSet<>();

    public Set<ConnectedEntity> getEntityConnections() {
        return entityConnections;
    }

    public void addEntityConnection(ConnectedEntity connectedEntity) {
        requiresNonNull(connectedEntity, "connectedEntity");
        entityConnections.add(connectedEntity);
        if (!this.equals(connectedEntity.getEntityLink())) {
            connectedEntity.setEntityLink(this);
        }
    }

    public void unlinkEntityConnection(ConnectedEntity connectedEntity) {
        if (entityConnections.contains(connectedEntity)) {
            entityConnections.remove(connectedEntity);
            if (connectedEntity != null) {
                connectedEntity.unlinkEntityLink(this);
            }
        }
    }

    public boolean isPresent() {
        return getStatus() != null && !statusOf(this).isUnavailableOffline().verify();
    }

    @PreRemove
    public void preRemoveDiscoverableEntity() {
        unlinkCollection(entityConnections, this::unlinkEntityConnection);
    }

    @Override
    public String toString() {
        return format("DiscoverableEntity {clazz=%s, uri=%s, primaryKey=%d}", getClass().getSimpleName(), getUri(), getPrimaryKey());
    }

    public boolean isAvailable() {
        return statusOf(this).isEnabled().isHealthy().verify();
    }

    public boolean isDegraded() {
        return this.getStatus() == null
            || !statusOf(this).isEnabled().verify()
            || statusOf(this).isCritical().verify();
    }
}
