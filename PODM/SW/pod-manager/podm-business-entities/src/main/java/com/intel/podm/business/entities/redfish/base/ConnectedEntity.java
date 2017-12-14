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

package com.intel.podm.business.entities.redfish.base;

import com.intel.podm.business.entities.redfish.Endpoint;
import com.intel.podm.business.entities.redfish.embeddables.Identifier;
import com.intel.podm.business.entities.redfish.embeddables.PciId;
import com.intel.podm.common.types.EntityRole;
import com.intel.podm.common.types.EntityType;
import com.intel.podm.common.types.Id;
import org.hibernate.annotations.Generated;
import org.hibernate.proxy.HibernateProxy;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.ElementCollection;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.GenerationTime.INSERT;

@javax.persistence.Entity
@Table(name = "connected_entity", indexes = @Index(name = "idx_connected_entity_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public class ConnectedEntity extends DiscoverableEntity {
    @Generated(INSERT)
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "entity_role")
    @Enumerated(STRING)
    private EntityRole entityRole;

    @Column(name = "entity_type")
    @Enumerated(STRING)
    private EntityType entityType;

    @Column(name = "pci_function_number")
    private Integer pciFunctionNumber;

    @Column(name = "pci_class_code")
    private String pciClassCode;

    @Embedded
    private PciId pciId;

    @ElementCollection
    @CollectionTable(name = "connected_entity_identifier", joinColumns = @JoinColumn(name = "connected_entity_id"))
    @OrderColumn(name = "connected_entity_identifier_order")
    private Set<Identifier> identifiers = new HashSet<>();

    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "discoverable_entity_id")
    private DiscoverableEntity entityLink;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "endpoint_id")
    private Endpoint endpoint;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public EntityRole getEntityRole() {
        return entityRole;
    }

    public void setEntityRole(EntityRole entityRole) {
        this.entityRole = entityRole;
    }

    public EntityType getEntityType() {
        return entityType;
    }

    public void setEntityType(EntityType entityType) {
        this.entityType = entityType;
    }

    public Integer getPciFunctionNumber() {
        return pciFunctionNumber;
    }

    public void setPciFunctionNumber(Integer pciFunctionNumber) {
        this.pciFunctionNumber = pciFunctionNumber;
    }

    public String getPciClassCode() {
        return pciClassCode;
    }

    public void setPciClassCode(String pciClassCode) {
        this.pciClassCode = pciClassCode;
    }

    public PciId getPciId() {
        return pciId;
    }

    public void setPciId(PciId pciId) {
        this.pciId = pciId;
    }

    public Set<Identifier> getIdentifiers() {
        return identifiers;
    }

    public void addIdentifier(Identifier identifier) {
        identifiers.add(identifier);
    }

    public DiscoverableEntity getEntityLink() {
        if (entityLink instanceof HibernateProxy) {
            return (DiscoverableEntity) ((HibernateProxy) entityLink).getHibernateLazyInitializer().getImplementation();
        }
        return entityLink;
    }

    public void setEntityLink(DiscoverableEntity entity) {
        if (!Objects.equals(this.entityLink, entity)) {
            unlinkEntityLink(this.entityLink);
            this.entityLink = entity;
            if (entity != null) {
                entity.setConnectedEntity(this);
            }
        }
    }

    public void unlinkEntityLink(DiscoverableEntity entity) {
        if (Objects.equals(this.entityLink, entity)) {
            this.entityLink = null;
            if (entity != null) {
                entity.unlinkConnectedEntity(this);
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

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
