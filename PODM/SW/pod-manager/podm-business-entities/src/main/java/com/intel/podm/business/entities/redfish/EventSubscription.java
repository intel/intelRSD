/*
 * Copyright (c) 2017-2018 Intel Corporation
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
import com.intel.podm.business.entities.converters.IdToLongConverter;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.events.EventType;
import org.hibernate.annotations.Generated;

import javax.persistence.CollectionTable;
import javax.persistence.Column;
import javax.persistence.Convert;
import javax.persistence.ElementCollection;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.OrderColumn;
import javax.persistence.Table;
import java.util.ArrayList;
import java.util.List;

import static javax.persistence.EnumType.STRING;
import static org.hibernate.annotations.GenerationTime.INSERT;

@Table(name = "event_subscription", indexes = @Index(name = "idx_event_subscription_entity_id", columnList = "entity_id", unique = true))
@javax.persistence.Entity
@SuppressWarnings({"checkstyle:MethodCount"})
@Eventable
public class EventSubscription extends Entity {
    @Generated(INSERT)
    @Convert(converter = IdToLongConverter.class)
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_NUMERIC_COLUMN_DEFINITION, insertable = false, nullable = false)
    private Id entityId;

    @Column(name = "name")
    private String name;

    @Column(name = "description")
    private String description;

    @Column(name = "destination")
    private String destination;

    @Column(name = "subscription_context")
    private String subscriptionContext;

    @Column(name = "protocol")
    private String protocol;

    @ElementCollection
    @Enumerated(STRING)
    @CollectionTable(name = "event_subscription_event_type", joinColumns = @JoinColumn(name = "event_subscription_id"))
    @Column(name = "event_type")
    @OrderColumn(name = "event_type_order")
    private List<EventType> eventTypes = new ArrayList<>();

    @ElementCollection
    @CollectionTable(name = "event_subscription_origin_resource", joinColumns = @JoinColumn(name = "event_subscription_id"))
    @Column(name = "origin_resource")
    @OrderColumn(name = "origin_resource_order")
    private List<String> originResources = new ArrayList<>();

    public Id getId() {
        return entityId;
    }

    public void setId(Id id) {
        entityId = id;
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

    public String getDestination() {
        return destination;
    }

    public void setDestination(String destination) {
        this.destination = destination;
    }

    public String getSubscriptionContext() {
        return subscriptionContext;
    }

    public void setSubscriptionContext(String context) {
        this.subscriptionContext = context;
    }

    public String getProtocol() {
        return protocol;
    }

    public void setProtocol(String protocol) {
        this.protocol = protocol;
    }

    public List<EventType> getEventTypes() {
        return eventTypes;
    }

    public void addEventType(EventType eventType) {
        eventTypes.add(eventType);
    }

    public List<String> getOriginResources() {
        return originResources;
    }

    public void addOriginResource(String originResource) {
        originResources.add(originResource);
    }

    @Override
    public void preRemove() {
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return false;
    }
}
