/*
 * Copyright (c) 2015-2018 Intel Corporation
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

import com.intel.podm.business.entities.SuppressEvents;
import com.intel.podm.business.entities.redfish.ExternalLink;
import com.intel.podm.business.entities.redfish.ExternalService;
import com.intel.podm.business.entities.redfish.Redundancy;
import com.intel.podm.business.entities.redfish.embeddables.UnknownOem;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.State;
import com.intel.podm.common.types.Status;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.DiscriminatorColumn;
import javax.persistence.ElementCollection;
import javax.persistence.Index;
import javax.persistence.Inheritance;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OrderColumn;
import javax.persistence.PreRemove;
import javax.persistence.Table;
import java.net.URI;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static com.intel.podm.business.entities.redfish.base.StatusControl.statusOf;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static com.intel.podm.common.utils.IterableHelper.optionalSingle;
import static java.lang.String.format;
import static java.util.stream.Collectors.toList;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;
import static javax.persistence.InheritanceType.JOINED;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = DiscoverableEntity.GET_ENTITY_BY_SERVICE_AND_SOURCE_URI,
        query = "SELECT el.discoverableEntity FROM ExternalLink el WHERE el.externalService = :externalService AND el.sourceUri = :sourceUri"
    )
})
@Inheritance(strategy = JOINED)
@DiscriminatorColumn(name = "discriminator_class", length = 100)
@Table(name = "discoverable_entity", indexes = @Index(name = "idx_discoverable_entity_global_id", columnList = "global_id", unique = true))
@SuppressWarnings({"checkstyle:MethodCount"})
public abstract class DiscoverableEntity extends Entity {

    public static final String GET_ENTITY_BY_SERVICE_AND_SOURCE_URI = "GET_ENTITY_BY_SERVICE_AND_SOURCE_URI";
    protected static final Status ABSENT = new Status(State.ABSENT, null, null);

    @Column(name = "name")
    private String name;

    @Column(name = "description")
    private String description;

    @Column(name = "status")
    private Status status;

    @Column(name = "global_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id globalId;

    @Column(name = "is_complementary")
    private boolean isComplementary;

    @ElementCollection
    @CollectionTable(name = "unknown_oem", joinColumns = @JoinColumn(name = "entity_id"))
    @OrderColumn(name = "unknown_oem_order")
    private List<UnknownOem> unknownOems = new ArrayList<>();

    /**
     * Any DiscoverableEntity can act as a redundancy owner.
     * This attribute contains all redundancies owned by this entity;
     */
    @SuppressEvents
    @OneToMany(mappedBy = "redundancyOwner", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<Redundancy> ownedRedundancies = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "redundancy_member",
        joinColumns = {@JoinColumn(name = "redundancy_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "member_id", referencedColumnName = "id")})
    private Set<Redundancy> redundancies = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "entity_related_item",
        joinColumns = {@JoinColumn(name = "item_owner_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "item_id", referencedColumnName = "id")})
    private Set<DiscoverableEntity> relatedItems = new HashSet<>();

    @ManyToMany(mappedBy = "relatedItems", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<DiscoverableEntity> referencedBy = new HashSet<>();

    @OneToMany(mappedBy = "discoverableEntity", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ExternalLink> externalLinks = new HashSet<>();

    @OneToMany(mappedBy = "entityLink", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ConnectedEntity> entityConnections = new HashSet<>();

    public abstract Id getId();

    public abstract void setId(Id id);

    public Id getGlobalId() {
        return globalId;
    }

    public void setGlobalId(Id globalId) {
        this.globalId = globalId;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public Status getStatus() {
        return status;
    }

    public void setStatus(Status status) {
        this.status = status;
    }

    public URI getSourceUri() {
        return optionalSingle(externalLinks).map(ExternalLink::getSourceUri).orElse(null);
    }

    public boolean isComplementary() {
        return isComplementary;
    }

    public void setComplementary(boolean complementary) {
        isComplementary = complementary;
    }

    public List<UnknownOem> getUnknownOems() {
        return unknownOems;
    }

    public void addUnknownOem(UnknownOem unknownOem) {
        this.unknownOems.add(unknownOem);
    }

    public Set<Redundancy> getOwnedRedundancies() {
        return ownedRedundancies;
    }

    public void addOwnedRedundancy(Redundancy ownedRedundancy) {
        requiresNonNull(ownedRedundancy, "ownedRedundancy");

        this.ownedRedundancies.add(ownedRedundancy);
        if (!this.equals(ownedRedundancy.getRedundancyOwner())) {
            ownedRedundancy.setRedundancyOwner(this);
        }
    }

    public void unlinkOwnedRedundancy(Redundancy redundancy) {
        if (this.ownedRedundancies.contains(redundancy)) {
            this.ownedRedundancies.remove(redundancy);
            if (redundancy != null) {
                redundancy.unlinkRedundancyOwner(this);
            }
        }
    }

    public Set<Redundancy> getRedundancies() {
        return redundancies;
    }

    public void addRedundancy(Redundancy redundancy) {
        requiresNonNull(redundancy, "redundancy");

        redundancies.add(redundancy);
        if (!redundancy.getRedundancyMembers().contains(this)) {
            redundancy.addRedundancyMember(this);
        }
    }

    public void unlinkRedundancy(Redundancy redundancy) {
        if (this.redundancies.contains(redundancy)) {
            this.redundancies.remove(redundancy);
            if (redundancy != null) {
                redundancy.unlinkRedundancyMember(this);
            }
        }
    }

    public Set<ExternalLink> getExternalLinks() {
        return externalLinks;
    }

    public void addExternalLink(ExternalLink externalLink) {
        requiresNonNull(externalLink, "externalLink");

        externalLinks.add(externalLink);
        if (!this.equals(externalLink.getDiscoverableEntity())) {
            externalLink.setDiscoverableEntity(this);
        }
    }

    public void unlinkExternalLink(ExternalLink externalLink) {
        if (externalLinks.contains(externalLink)) {
            externalLinks.remove(externalLink);
            if (externalLink != null) {
                externalLink.unlinkDiscoverableEntity(this);
            }
        }
    }

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

    public Set<DiscoverableEntity> getRelatedItems() {
        return relatedItems;
    }

    public void addRelatedItem(DiscoverableEntity relatedItem) {
        requiresNonNull(relatedItem, "relatedItem");

        relatedItems.add(relatedItem);
        if (!relatedItem.getReferencedBy().contains(this)) {
            relatedItem.addReferencedBy(this);
        }
    }

    public void unlinkRelatedItem(DiscoverableEntity relatedItem) {
        if (this.relatedItems.contains(relatedItem)) {
            this.relatedItems.remove(relatedItem);
            if (relatedItem != null) {
                relatedItem.unlinkReferencedBy(this);
            }
        }
    }

    public Set<DiscoverableEntity> getReferencedBy() {
        return referencedBy;
    }

    public void addReferencedBy(DiscoverableEntity containedItem) {
        requiresNonNull(containedItem, "containedItem");

        referencedBy.add(containedItem);
        if (!containedItem.getRelatedItems().contains(this)) {
            containedItem.addRelatedItem(this);
        }
    }

    public void unlinkReferencedBy(DiscoverableEntity containedItem) {
        if (referencedBy.contains(containedItem)) {
            referencedBy.remove(containedItem);
            if (containedItem != null) {
                containedItem.unlinkRelatedItem(this);
            }
        }
    }

    public ExternalService getService() {
        return optionalSingle(getExternalLinks())
            .map(ExternalLink::getExternalService)
            .orElse(null);
    }

    public boolean isPresent() {
        return getStatus() != null && !statusOf(this).isAbsent().verify();
    }

    public void putToAbsentState() {
        setStatus(ABSENT);
    }

    @PreRemove
    public void preRemoveDiscoverableEntity() {
        unlinkCollection(ownedRedundancies, this::unlinkOwnedRedundancy);
        unlinkCollection(redundancies, this::unlinkRedundancy);
        unlinkCollection(externalLinks, this::unlinkExternalLink);
        unlinkCollection(entityConnections, this::unlinkEntityConnection);
        unlinkCollection(relatedItems, this::unlinkRelatedItem);
        unlinkCollection(referencedBy, this::unlinkReferencedBy);
    }

    @Override
    public String toString() {
        return format("DiscoverableEntity {clazz=%s, entityId=%s, primaryKey=%d, externalLinks=%s}",
            getClass().getSimpleName(), getId(), getPrimaryKey(), getExternalLinks());
    }

    public List<ExternalService> getExternalServices() {
        return getExternalLinks().stream().map(ExternalLink::getExternalService).collect(toList());
    }

    public boolean hasAnyExternalServiceAssigned() {
        return !getExternalLinks().isEmpty();
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
