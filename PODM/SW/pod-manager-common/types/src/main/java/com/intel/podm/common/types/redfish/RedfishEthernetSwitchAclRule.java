/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.common.types.redfish;

import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;
import com.intel.podm.common.types.Ref;

import java.util.Set;

public interface RedfishEthernetSwitchAclRule extends RedfishResource {
    Ref<Integer> getRuleId();
    Ref<ActionType> getActionType();
    Ref<? extends NavigationProperty> getForwardMirrorInterface();
    Ref<? extends Set<? extends NavigationProperty>> getMirrorPortRegions();
    Ref<MirrorType> getMirrorType();
    Ref<? extends AclRuleCondition> getCondition();

    interface AclRuleCondition {
        Ref<? extends AclRuleConditionIp> getIpSource();
        Ref<? extends AclRuleConditionIp> getIpDestination();
        Ref<? extends AclRuleConditionMacAddress> getMacSource();
        Ref<? extends AclRuleConditionMacAddress> getMacDestination();
        Ref<? extends AclRuleConditionId> getVlanId();
        Ref<? extends AclRuleConditionPort> getL4SourcePort();
        Ref<? extends AclRuleConditionPort> getL4DestinationPort();
        Ref<Integer> getL4Protocol();
    }

    interface AclRuleConditionIp {
        Ref<String> getIpv4Address();
        Ref<String> getMask();
    }

    interface AclRuleConditionMacAddress {
        Ref<String> getMacAddress();
        Ref<String> getMask();
    }

    interface AclRuleConditionId {
        Ref<Long> getId();
        Ref<Long> getMask();
    }

    interface AclRuleConditionPort {
        Ref<Long> getPort();
        Ref<Long> getMask();
    }
}
