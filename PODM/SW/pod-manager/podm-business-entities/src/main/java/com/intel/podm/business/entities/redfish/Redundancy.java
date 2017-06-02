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
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "redundancy", indexes = @Index(name = "idx_redundancy_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings({"checkstyle:MethodCount"})
public class Redundancy extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "member_id")
    private String memberId;

    @Column(name = "redundancy_enabled")
    private Boolean redundancyEnabled;

    @Column(name = "mode")
    private String mode;

    @Column(name = "min_num_needed")
    private Integer minNumNeeded;

    @Column(name = "max_num_supported")
    private Integer maxNumSupported;

    @ManyToMany(mappedBy = "redundancies", fetch = LAZY, cascade = {MERGE, PERSIST})
    private Set<DiscoverableEntity> redundancyMembers = new HashSet<>();

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "redundancy_owner_id")
    private DiscoverableEntity redundancyOwner;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        this.entityId = id;
    }

    public String getMemberId() {
        return memberId;
    }

    public void setMemberId(String memberId) {
        this.memberId = memberId;
    }

    public Boolean getRedundancyEnabled() {
        return redundancyEnabled;
    }

    public void setRedundancyEnabled(Boolean redundancyEnabled) {
        this.redundancyEnabled = redundancyEnabled;
    }

    public String getMode() {
        return mode;
    }

    public void setMode(String mode) {
        this.mode = mode;
    }

    public Integer getMinNumNeeded() {
        return minNumNeeded;
    }

    public void setMinNumNeeded(Integer minNumNeeded) {
        this.minNumNeeded = minNumNeeded;
    }

    public Integer getMaxNumSupported() {
        return maxNumSupported;
    }

    public void setMaxNumSupported(Integer maxNumSupported) {
        this.maxNumSupported = maxNumSupported;
    }

    public Set<DiscoverableEntity> getRedundancyMembers() {
        return redundancyMembers;
    }

    public void addRedundancyMember(DiscoverableEntity redundancyMember) {
        requiresNonNull(redundancyMember, "redundancyMember");

        redundancyMembers.add(redundancyMember);
        if (!redundancyMember.getRedundancies().contains(this)) {
            redundancyMember.addRedundancy(this);
        }
    }

    public void unlinkRedundancyMember(DiscoverableEntity redundancyMember) {
        if (redundancyMembers.contains(redundancyMember)) {
            redundancyMembers.remove(redundancyMember);
            if (redundancyMember != null) {
                redundancyMember.unlinkRedundancy(this);
            }
        }
    }

    public DiscoverableEntity getRedundancyOwner() {
        return redundancyOwner;
    }

    public void setRedundancyOwner(DiscoverableEntity redundancyOwner) {
        if (!Objects.equals(this.redundancyOwner, redundancyOwner)) {
            unlinkRedundancyOwner(this.redundancyOwner);
            this.redundancyOwner = redundancyOwner;
            if (redundancyOwner != null && !redundancyOwner.getOwnedRedundancies().contains(this)) {
                redundancyOwner.addOwnedRedundancy(this);
            }
        }
    }

    public void unlinkRedundancyOwner(DiscoverableEntity redundancyOwner) {
        if (Objects.equals(this.redundancyOwner, redundancyOwner)) {
            this.redundancyOwner = null;
            if (redundancyOwner != null) {
                redundancyOwner.unlinkOwnedRedundancy(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(redundancyMembers, this::unlinkRedundancyMember);
        unlinkRedundancyOwner(redundancyOwner);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, redundancyOwner);
    }
}
