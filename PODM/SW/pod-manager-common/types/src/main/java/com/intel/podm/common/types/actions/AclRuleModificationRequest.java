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

package com.intel.podm.common.types.actions;

import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleCondition;

import java.net.URI;
import java.util.Set;

public final class AclRuleModificationRequest {
    private Ref<Integer> ruleId;
    private Ref<ActionType> action;
    private Ref<URI> forwardMirrorInterface;
    private Ref<Set<URI>> mirrorPortRegion;
    private Ref<MirrorType> mirrorType;
    private Ref<? extends AclRuleCondition> condition;

    private AclRuleModificationRequest(Builder builder) {
        this.ruleId = builder.getRuleId();
        this.action = builder.getAction();
        this.forwardMirrorInterface = builder.getForwardMirrorInterface();
        this.mirrorPortRegion = builder.getMirrorPortRegion();
        this.mirrorType = builder.getMirrorType();
        this.condition = builder.getCondition();
    }

    public Ref<Integer> getRuleId() {
        return ruleId;
    }

    public Ref<ActionType> getAction() {
        return action;
    }

    public Ref<URI> getForwardMirrorInterface() {
        return forwardMirrorInterface;
    }

    public Ref<Set<URI>> getMirrorPortRegion() {
        return mirrorPortRegion;
    }

    public Ref<MirrorType> getMirrorType() {
        return mirrorType;
    }

    public Ref<? extends AclRuleCondition> getCondition() {
        return condition;
    }

    public static class Builder {
        private Ref<Integer> ruleId;
        private Ref<ActionType> action;
        private Ref<URI> forwardMirrorInterface;
        private Ref<Set<URI>> mirrorPortRegion;
        private Ref<MirrorType> mirrorType;
        private Ref<? extends AclRuleCondition> condition;

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

        public Builder setActionType(Ref<ActionType> action) {
            this.action = action;
            return this;
        }

        public Ref<MirrorType> getMirrorType() {
            return mirrorType;
        }

        public Builder setMirrorType(Ref<MirrorType> mirrorType) {
            this.mirrorType = mirrorType;
            return this;
        }

        public Ref<URI> getForwardMirrorInterface() {
            return forwardMirrorInterface;
        }

        public Builder setForwardMirrorInterface(Ref<URI> forwardMirrorInterface) {
            this.forwardMirrorInterface = forwardMirrorInterface;
            return this;
        }

        public Ref<Set<URI>> getMirrorPortRegion() {
            return mirrorPortRegion;
        }

        public Builder setMirrorPortRegion(Ref<Set<URI>> mirrorPortRegion) {
            this.mirrorPortRegion = mirrorPortRegion;
            return this;
        }

        public Ref<? extends AclRuleCondition> getCondition() {
            return condition;
        }

        public Builder setCondition(Ref<? extends AclRuleCondition> condition) {
            this.condition = condition;
            return this;
        }

        public AclRuleModificationRequest build() {
            return new AclRuleModificationRequest(this);
        }
    }
}
