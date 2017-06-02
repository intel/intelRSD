/*
 * Copyright (c) 2015-2017 Intel Corporation
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
import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.listeners.RemoteTargetListener;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;

import javax.persistence.Column;
import javax.persistence.EntityListeners;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Optional;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.EAGER;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@NamedQueries({
    @NamedQuery(name = RemoteTarget.GET_REMOTE_TARGET_BY_SOURCE_URI,
        query = "SELECT rt FROM RemoteTarget rt WHERE rt.sourceUri = :sourceUri")
})
@Table(name = "remote_target", indexes = @Index(name = "idx_remote_target_entity_id", columnList = "entity_id", unique = true))
@EntityListeners(RemoteTargetListener.class)
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class RemoteTarget extends DiscoverableEntity {
    public static final String GET_REMOTE_TARGET_BY_SOURCE_URI = "GET_REMOTE_TARGET_BY_SOURCE_URI";

    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "iscsi_initiator_iqn")
    private String iscsiInitiatorIqn;

    @Column(name = "type")
    private String type;

    @OneToMany(mappedBy = "remoteTarget", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<RemoteTargetIscsiAddress> remoteTargetIscsiAddresses = new HashSet<>();

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "remote_target_logical_drive",
        joinColumns = {@JoinColumn(name = "remote_target_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "logical_drive_id", referencedColumnName = "id")})
    private Set<LogicalDrive> logicalDrives = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = EAGER, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "remote_target_metadata_id")
    private RemoteTargetMetadata metadata = new RemoteTargetMetadata();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "storage_service_id")
    private StorageService storageService;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "composed_node_id")
    private ComposedNode composedNode;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public String getIscsiInitiatorIqn() {
        return iscsiInitiatorIqn;
    }

    public void setIscsiInitiatorIqn(String iscsiInitiatorIqn) {
        this.iscsiInitiatorIqn = iscsiInitiatorIqn;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    public Set<RemoteTargetIscsiAddress> getRemoteTargetIscsiAddresses() {
        return remoteTargetIscsiAddresses;
    }

    public void addRemoteTargetIscsiAddress(RemoteTargetIscsiAddress remoteTargetIscsiAddress) {
        requiresNonNull(remoteTargetIscsiAddress, "remoteTargetIscsiAddress");

        remoteTargetIscsiAddresses.add(remoteTargetIscsiAddress);
        if (!this.equals(remoteTargetIscsiAddress.getRemoteTarget())) {
            remoteTargetIscsiAddress.setRemoteTarget(this);
        }
    }

    public void unlinkRemoteTargetIscsiAddress(RemoteTargetIscsiAddress remoteTargetIscsiAddress) {
        if (remoteTargetIscsiAddresses.contains(remoteTargetIscsiAddress)) {
            remoteTargetIscsiAddresses.remove(remoteTargetIscsiAddress);
            if (remoteTargetIscsiAddress != null) {
                remoteTargetIscsiAddress.unlinkRemoteTarget(this);
            }
        }
    }

    public String getTargetIqn() {
        Optional<RemoteTargetIscsiAddress> firstIscsiAddress = getRemoteTargetIscsiAddresses().stream().findFirst();

        if (!firstIscsiAddress.isPresent()) {
            return null;
        }

        return firstIscsiAddress.get().getTargetIqn();
    }

    public Set<LogicalDrive> getLogicalDrives() {
        return logicalDrives;
    }

    public void addLogicalDrive(LogicalDrive logicalDrive) {
        requiresNonNull(logicalDrive, "logicalDrive");

        logicalDrives.add(logicalDrive);
        if (!logicalDrive.getRemoteTargets().contains(this)) {
            logicalDrive.addRemoteTarget(this);
        }
    }

    public void unlinkLogicalDrive(LogicalDrive logicalDrive) {
        if (logicalDrives.contains(logicalDrive)) {
            logicalDrives.remove(logicalDrive);
            if (logicalDrive != null) {
                logicalDrive.unlinkRemoteTarget(this);
            }
        }
    }

    public RemoteTargetMetadata getMetadata() {
        return metadata;
    }

    public StorageService getStorageService() {
        return storageService;
    }

    public void setStorageService(StorageService storageService) {
        if (!Objects.equals(this.storageService, storageService)) {
            unlinkStorageService(this.storageService);
            this.storageService = storageService;
            if (storageService != null && !storageService.getRemoteTargets().contains(this)) {
                storageService.addRemoteTarget(this);
            }
        }
    }

    public void unlinkStorageService(StorageService storageService) {
        if (Objects.equals(this.storageService, storageService)) {
            this.storageService = null;
            if (storageService != null) {
                storageService.unlinkRemoteTarget(this);
            }
        }
    }

    public ComposedNode getComposedNode() {
        return composedNode;
    }

    public void setComposedNode(ComposedNode composedNode) {
        if (!Objects.equals(this.composedNode, composedNode)) {
            unlinkComposedNode(this.composedNode);
            this.composedNode = composedNode;
            if (composedNode != null && !composedNode.getRemoteTargets().contains(this)) {
                composedNode.addRemoteTarget(this);
            }
        }
    }

    public void unlinkComposedNode(ComposedNode composedNode) {
        if (Objects.equals(this.composedNode, composedNode)) {
            this.composedNode = null;
            if (composedNode != null) {
                composedNode.unlinkRemoteTarget(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(remoteTargetIscsiAddresses, this::unlinkRemoteTargetIscsiAddress);
        unlinkCollection(logicalDrives, this::unlinkLogicalDrive);
        unlinkStorageService(storageService);
        unlinkComposedNode(composedNode);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, storageService);
    }
}
