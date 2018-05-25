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

package com.intel.podm.business.entities.redfish;

import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.converters.IdToLongConverter;
import com.intel.podm.business.entities.listeners.ExternalServiceListener;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.ServiceType;
import org.hibernate.annotations.Generated;

import javax.persistence.Column;
import javax.persistence.Convert;
import javax.persistence.EntityListeners;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import java.net.URI;
import java.time.Duration;
import java.time.LocalDateTime;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.UUID;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.time.Duration.between;
import static java.time.LocalDateTime.now;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;
import static org.hibernate.annotations.GenerationTime.INSERT;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = ExternalService.GET_EXTERNAL_SERVICE_BY_UUID,
        query = "SELECT es FROM ExternalService es WHERE es.uuid = :uuid"),
    @NamedQuery(name = ExternalService.GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES,
        query = "SELECT es FROM ExternalService es WHERE es.serviceType IN (:serviceTypes)")
})
@Table(name = "external_service", indexes = {
    @Index(name = "idx_external_service_entity_id", columnList = "entity_id", unique = true),
    @Index(name = "idx_external_service_uuid", columnList = "uuid", unique = true)
})
@EntityListeners(ExternalServiceListener.class)
@SuppressWarnings({"checkstyle:MethodCount"})
public class ExternalService extends Entity {
    public static final String GET_EXTERNAL_SERVICE_BY_UUID = "GET_EXTERNAL_SERVICE_BY_UUID";
    public static final String GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES = "GET_EXTERNAL_SERVICES_BY_SERVICES_TYPES";

    @Generated(INSERT)
    @Convert(converter = IdToLongConverter.class)
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_NUMERIC_COLUMN_DEFINITION, insertable = false, nullable = false)
    private Id entityId;

    @Column(name = "uuid")
    private UUID uuid;

    @Column(name = "uri")
    private URI baseUri;

    @Column(name = "service_type")
    @Enumerated(STRING)
    private ServiceType serviceType;

    @Column(name = "is_reachable")
    private boolean isReachable = true;

    @Column(name = "unreachable_since")
    private LocalDateTime unreachableSince;

    @Column(name = "is_complementary_data_source")
    private boolean isComplementaryDataSource;

    @Column(name = "is_eventing_available")
    private boolean eventingAvailable = true;

    @IgnoreUnlinkingRelationship
    @OneToMany(mappedBy = "externalService", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<ExternalLink> ownedLinks = new HashSet<>();

    public Id getId() {
        return entityId;
    }

    public UUID getUuid() {
        return uuid;
    }

    public void setUuid(UUID uuid) {
        this.uuid = uuid;
    }

    public URI getBaseUri() {
        return baseUri;
    }

    public void setBaseUri(URI baseUri) {
        this.baseUri = baseUri;
    }

    public ServiceType getServiceType() {
        return serviceType;
    }

    public void setServiceType(ServiceType serviceType) {
        this.serviceType = serviceType;
    }

    public boolean isReachable() {
        return isReachable;
    }

    public boolean isUnreachableLongerThan(Duration duration) {
        requiresNonNull(duration, "duration");
        return !isReachable() && getHowLongIsUnreachable().compareTo(duration) > 0;
    }

    public void markAsNotReachable() {
        getOwnedLinks().stream()
            .map(ExternalLink::getDiscoverableEntity)
            .filter(this::allOtherRelatedServices)
            .forEach(DiscoverableEntity::putToAbsentState);

        if (!isReachable()) {
            return;
        }

        unreachableSince = now();
        isReachable = false;
    }

    private boolean allOtherRelatedServices(DiscoverableEntity entity) {
        return entity.getExternalLinks().stream()
            .filter(externalLink -> externalLink.getExternalService() != this)
            .map(ExternalLink::getExternalService)
            .noneMatch(ExternalService::isReachable);
    }

    public void markAsReachable() {
        isReachable = true;
        unreachableSince = null;
    }

    private Duration getHowLongIsUnreachable() {
        return between(unreachableSince, now());
    }

    public boolean isComplementaryDataSource() {
        return isComplementaryDataSource;
    }

    public void setComplementaryDataSource(boolean complementaryDataSource) {
        isComplementaryDataSource = complementaryDataSource;
    }

    public boolean isEventingAvailable() {
        return eventingAvailable;
    }

    public void setEventingAvailable(boolean eventingAvailable) {
        this.eventingAvailable = eventingAvailable;
    }

    @SuppressWarnings({"unchecked"})
    public <T extends DiscoverableEntity> List<T> getOwned(Class<T> clazz) {
        return ownedLinks.stream()
            .map(ExternalLink::getDiscoverableEntity)
            .filter(clazz::isInstance)
            .map(entity -> (T) entity)
            .collect(toList());
    }

    public Set<ExternalLink> getOwnedLinks() {
        return ownedLinks;
    }

    public void addOwnedLink(ExternalLink externalLink) {
        requiresNonNull(externalLink, "entity");

        ownedLinks.add(externalLink);
        if (!this.equals(externalLink.getExternalService())) {
            externalLink.setExternalService(this);
        }
    }

    public void unlinkOwnedLink(ExternalLink externalLink) {
        if (ownedLinks.contains(externalLink)) {
            ownedLinks.remove(externalLink);
            if (externalLink != null) {
                externalLink.unlinkExternalService(this);
            }
        }
    }

    @Override
    public void preRemove() {
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }

    @Override
    public String toString() {
        if (isReachable()) {
            return format("ExternalService {UUID=%s, baseUri=%s, type=%s}",
                getUuid(),
                getBaseUri(),
                getServiceType());
        } else {
            return format("ExternalService {UUID=%s, baseUri=%s, type=%s, unreachableSince=%s}",
                getUuid(),
                getBaseUri(),
                getServiceType(),
                unreachableSince);
        }
    }

    public Set<DiscoverableEntity> getOwnedEntities() {
        return getOwnedLinks().stream().map(ExternalLink::getDiscoverableEntity).collect(toSet());
    }
}
