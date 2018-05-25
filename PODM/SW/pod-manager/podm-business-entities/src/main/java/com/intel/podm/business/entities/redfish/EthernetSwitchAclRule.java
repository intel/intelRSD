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
import com.intel.podm.business.entities.IgnoreUnlinkingRelationship;
import com.intel.podm.business.entities.redfish.base.DiscoverableEntity;
import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.redfish.embeddables.Condition;
import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.Id;
import com.intel.podm.common.types.MirrorType;

import javax.persistence.Column;
import javax.persistence.Embedded;
import javax.persistence.Enumerated;
import javax.persistence.Index;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.function.Predicate;

import static com.intel.podm.common.utils.Contracts.requiresNonNull;
import static javax.persistence.CascadeType.MERGE;
import static javax.persistence.CascadeType.PERSIST;
import static javax.persistence.EnumType.STRING;
import static javax.persistence.FetchType.LAZY;

@javax.persistence.Entity
@Table(name = "ethernet_switch_acl_rule", indexes = @Index(name = "idx_ethernet_switch_acl_rule_entity_id", columnList = "entity_id", unique = true))
@Eventable
@SuppressWarnings("checkstyle:MethodCount")
public class EthernetSwitchAclRule extends DiscoverableEntity {
    @Column(name = "entity_id", columnDefinition = ENTITY_ID_STRING_COLUMN_DEFINITION)
    private Id entityId;

    @Column(name = "rule_id")
    private Long ruleId;

    @Column(name = "action")
    @Enumerated(STRING)
    private ActionType action;

    @Column(name = "mirror_type")
    @Enumerated(STRING)
    private MirrorType mirrorType;

    @Embedded
    private Condition condition;

    @ManyToMany(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinTable(
        name = "ethernet_switch_acl_rule_mirror_port_region",
        joinColumns = {@JoinColumn(name = "rule_id", referencedColumnName = "id")},
        inverseJoinColumns = {@JoinColumn(name = "port_id", referencedColumnName = "id")}
    )
    private Set<EthernetSwitchPort> mirrorPortRegions = new HashSet<>();

    @IgnoreUnlinkingRelationship
    @OneToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_acl_rule_forward_mirror_interface_id")
    private EthernetSwitchPort forwardMirrorInterface;

    @ManyToOne(fetch = LAZY, cascade = {MERGE, PERSIST})
    @JoinColumn(name = "ethernet_switch_acl_id")
    private EthernetSwitchAcl ethernetSwitchAcl;

    @Override
    public Id getId() {
        return entityId;
    }

    @Override
    public void setId(Id id) {
        entityId = id;
    }

    public Long getRuleId() {
        return ruleId;
    }

    public void setRuleId(Long ruleId) {
        this.ruleId = ruleId;
    }

    public ActionType getAction() {
        return action;
    }

    public void setAction(ActionType action) {
        this.action = action;
    }

    public MirrorType getMirrorType() {
        return mirrorType;
    }

    public void setMirrorType(MirrorType mirrorType) {
        this.mirrorType = mirrorType;
    }

    public Condition getCondition() {
        return condition;
    }

    public void setCondition(Condition condition) {
        this.condition = condition;
    }

    public Set<EthernetSwitchPort> getMirrorPortRegions() {
        return mirrorPortRegions;
    }

    public void addMirrorPort(EthernetSwitchPort mirrorPort) {
        requiresNonNull(mirrorPort, "mirrorPort");

        this.mirrorPortRegions.add(mirrorPort);
        if (!mirrorPort.getMirrorPortRegionForRules().contains(this)) {
            mirrorPort.addMirrorPortRegionForRules(this);
        }
    }

    public void unlinkMirrorPortRegion(EthernetSwitchPort mirrorPort) {
        if (mirrorPortRegions.contains(mirrorPort)) {
            mirrorPortRegions.remove(mirrorPort);
            if (mirrorPort != null) {
                mirrorPort.unlinkMirrorPortRegionForRules(this);
            }
        }
    }

    public void uncoupleMirrorPortRegion(Predicate<EthernetSwitchPort> unlinkPredicate) {
        unlinkCollection(mirrorPortRegions, this::unlinkMirrorPortRegion, unlinkPredicate);
    }

    public EthernetSwitchPort getForwardMirrorInterface() {
        return forwardMirrorInterface;
    }

    public void setForwardMirrorInterface(EthernetSwitchPort forwardMirrorInterface) {
        if (!Objects.equals(this.forwardMirrorInterface, forwardMirrorInterface)) {
            unlinkForwardMirrorInterface(this.forwardMirrorInterface);
            this.forwardMirrorInterface = forwardMirrorInterface;
        }
    }

    public void unlinkForwardMirrorInterface(EthernetSwitchPort forwardMirrorInterface) {
        if (Objects.equals(this.forwardMirrorInterface, forwardMirrorInterface)) {
            this.forwardMirrorInterface = null;
        }
    }

    public EthernetSwitchAcl getEthernetSwitchAcl() {
        return ethernetSwitchAcl;
    }

    public void setEthernetSwitchAcl(EthernetSwitchAcl ethernetSwitchAcl) {
        if (!Objects.equals(this.ethernetSwitchAcl, ethernetSwitchAcl)) {
            unlinkEthernetSwitchAcl(this.ethernetSwitchAcl);
            this.ethernetSwitchAcl = ethernetSwitchAcl;
            if (ethernetSwitchAcl != null && !ethernetSwitchAcl.getRules().contains(this)) {
                ethernetSwitchAcl.addRule(this);
            }
        }
    }

    public void unlinkEthernetSwitchAcl(EthernetSwitchAcl ethernetSwitchAcl) {
        if (Objects.equals(this.ethernetSwitchAcl, ethernetSwitchAcl)) {
            this.ethernetSwitchAcl = null;
            if (ethernetSwitchAcl != null) {
                ethernetSwitchAcl.unlinkRule(this);
            }
        }
    }

    @Override
    public void preRemove() {
        unlinkCollection(mirrorPortRegions, this::unlinkMirrorPortRegion);
        unlinkForwardMirrorInterface(forwardMirrorInterface);
        unlinkEthernetSwitchAcl(ethernetSwitchAcl);
    }

    @Override
    public boolean containedBy(Entity possibleParent) {
        return isContainedBy(possibleParent, ethernetSwitchAcl);
    }
}
