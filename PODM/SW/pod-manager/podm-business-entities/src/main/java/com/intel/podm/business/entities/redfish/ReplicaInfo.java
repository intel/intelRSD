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

import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.ReplicaRole;
import com.intel.podm.common.types.ReplicaType;

import javax.persistence.Column;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import java.util.Objects;

import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.stream.Collectors.toSet;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "replica_info", indexes = @Index(name = "idx_replica_info_entity_id", columnList = "entity_id", unique = true))
@SuppressWarnings("checkstyle:MethodCount")
public class ReplicaInfo extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "replica_type")
    @Enumerated(STRING)
    private ReplicaType replicaType;

    @Column(name = "replica_role")
    @Enumerated(STRING)
    private ReplicaRole replicaRole;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "volume_id")
    private Volume volume;

    @IgnoreUnlinkingRelationship
    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "replica_id")
    private Volume replica;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id entityId) {
        this.entityId = entityId;
    }

    public Volume getVolume() {
        return volume;
    }

    public void setVolume(Volume volume) {
        this.volume = volume;
    }

    public void unlinkVolume(Volume volume) {
        if (Objects.equals(this.volume, volume)) {
            this.volume = null;
            if (volume != null) {
                volume.unlinkReplicaInfo(this);
            }
        }
    }

    public ReplicaType getReplicaType() {
        return replicaType;
    }

    public void setReplicaType(ReplicaType replicaType) {
        this.replicaType = replicaType;
    }

    public ReplicaRole getReplicaRole() {
        return replicaRole;
    }

    public void setReplicaRole(ReplicaRole replicaRole) {
        this.replicaRole = replicaRole;
    }

    public Volume getReplica() {
        return replica;
    }

    public void setReplica(Volume replica) {
        this.replica = replica;
    }

    public void unlinkReplica(Volume replica) {
        if (Objects.equals(this.replica, replica)) {
            this.replica = null;
            if (replica != null) {
                ReplicaInfo ri = singleOrNull(replica.getReplicaInfos().stream()
                    .filter(replicaInfo -> volume.equals(replicaInfo.getReplica()))
                    .collect(toSet()));

                replica.unlinkReplicaInfo(ri);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkReplica(replica);
        unlinkVolume(volume);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, volume);
    }
}
