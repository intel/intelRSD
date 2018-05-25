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

package com.intel.podm.client.actions;

import com.fasterxml.jackson.annotation.JsonInclude;
import com.fasterxml.jackson.annotation.JsonProperty;
import com.intel.podm.client.resources.ODataId;
import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleCondition;

import java.net.URI;
import java.util.Set;

import static com.fasterxml.jackson.annotation.JsonInclude.Include.NON_DEFAULT;
import static com.intel.podm.client.actions.EthernetSwitchAclRuleCondition.newEthernetSwitchAclRuleCondition;
import static com.intel.podm.common.types.Ref.of;
import static com.intel.podm.common.types.Ref.unassigned;
import static java.util.stream.Collectors.toSet;

@JsonInclude(NON_DEFAULT)
public final class RuleModificationRequest {
    @JsonProperty("RuleId")
    private Ref<Integer> ruleId = unassigned();

    @JsonProperty("Action")
    private Ref<ActionType> action = unassigned();

    @JsonProperty("ForwardMirrorInterface")
    private Ref<ODataId> forwardMirrorInterface = unassigned();

    @JsonProperty("MirrorPortRegion")
    private Ref<Set<ODataId>> mirrorPortRegions = unassigned();

    @JsonProperty("MirrorType")
    private Ref<MirrorType> mirrorType = unassigned();

    @JsonProperty("Condition")
    private Ref<EthernetSwitchAclRuleCondition> condition = unassigned();

    public RuleModificationRequest() {
    }

    private RuleModificationRequest(Builder builder) {
        this.ruleId = builder.getRuleId();
        this.action = builder.getAction();
        this.forwardMirrorInterface = builder.getForwardMirrorInterface();
        this.mirrorPortRegions = builder.getMirrorPortRegions();
        this.mirrorType = builder.getMirrorType();
        this.condition = builder.getCondition();
    }

    public static class Builder {
        private Ref<Integer> ruleId = unassigned();
        private Ref<ActionType> action = unassigned();
        private Ref<ODataId> forwardMirrorInterface = unassigned();
        private Ref<Set<ODataId>> mirrorPortRegions = unassigned();
        private Ref<MirrorType> mirrorType = unassigned();
        private Ref<EthernetSwitchAclRuleCondition> condition = unassigned();

        public Ref<Integer> getRuleId() {
            return ruleId;
        }

        public Builder setRuleId(Ref<Integer> ruleId) {
            this.ruleId = ruleId;

            return this;
        }

        public Ref<ActionType> getAction() {
            return action;
        }

        public Builder setAction(Ref<ActionType> action) {
            this.action = action;

            return this;
        }

        public Ref<ODataId> getForwardMirrorInterface() {
            return forwardMirrorInterface;
        }

        public Builder setForwardMirrorInterface(Ref<URI> forwardMirrorInterface) {
            forwardMirrorInterface.ifAssigned(uri -> this.forwardMirrorInterface = of(uri != null ? new ODataId(uri) : null));
            return this;
        }

        public Ref<Set<ODataId>> getMirrorPortRegions() {
            return mirrorPortRegions;
        }

        public Builder setMirrorPortRegions(Ref<Set<URI>> mirrorPortRegions) {
            mirrorPortRegions.ifAssigned(mirrorPortsUri ->
                this.mirrorPortRegions = of(mirrorPortsUri != null ? getODataIdFromMirrorPortCollect(mirrorPortsUri) : null));

            return this;
        }

        private Set<ODataId> getODataIdFromMirrorPortCollect(Set<URI> mirrorPortsUri) {
            return mirrorPortsUri.stream()
                .map(ODataId::new)
                .collect(toSet());
        }

        public Ref<MirrorType> getMirrorType() {
            return mirrorType;
        }

        public Builder setMirrorType(Ref<MirrorType> mirrorType) {
            this.mirrorType = mirrorType;

            return this;
        }

        public Ref<EthernetSwitchAclRuleCondition> getCondition() {
            return condition;
        }

        public Builder setCondition(Ref<? extends AclRuleCondition> orgConditionRef) {
            orgConditionRef.ifAssigned(orgCondition -> this.condition = of(orgCondition != null ? buildAclRuleCondition(orgCondition) : null));
            return this;
        }

        private EthernetSwitchAclRuleCondition buildAclRuleCondition(AclRuleCondition aclRuleCondition) {
            return newEthernetSwitchAclRuleCondition()
                .ipDestination(aclRuleCondition.getIpDestination())
                .ipSource(aclRuleCondition.getIpSource())
                .l4DestinationPort(aclRuleCondition.getL4DestinationPort())
                .l4SourcePort(aclRuleCondition.getL4SourcePort())
                .l4Protocol(aclRuleCondition.getL4Protocol())
                .macDestination(aclRuleCondition.getMacDestination())
                .macSource(aclRuleCondition.getMacSource())
                .vlanId(aclRuleCondition.getVlanId())
                .build();
        }

        public RuleModificationRequest build() {
            return new RuleModificationRequest(this);
        }
    }
}
