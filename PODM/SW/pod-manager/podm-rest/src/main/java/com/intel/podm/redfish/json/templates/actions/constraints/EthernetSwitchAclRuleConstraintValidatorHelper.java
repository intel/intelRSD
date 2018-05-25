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

package com.intel.podm.redfish.json.templates.actions.constraints;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleCondition;
import com.intel.podm.redfish.json.templates.actions.AclRuleModificationJson;

import javax.validation.ConstraintValidatorContext;
import java.util.Set;

public final class EthernetSwitchAclRuleConstraintValidatorHelper {

    private EthernetSwitchAclRuleConstraintValidatorHelper() {

    }

    public static boolean isNotEmptyRequest(AclRuleModificationJson aclRuleModificationJson, ConstraintValidatorContext context) {
        boolean valid = true;
        AclRuleModificationJson jsonWithUnsetProperties = new AclRuleModificationJson();
        if (jsonWithUnsetProperties.equals(aclRuleModificationJson)) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Request is empty - cannot continue").addConstraintViolation();
        }
        return valid;
    }

    public static boolean checkNonNulls(AclRuleModificationJson aclRuleModificationJson, ConstraintValidatorContext context) {
        boolean valid = true;
        ActionType actionType = getActionType(aclRuleModificationJson);
        AclRuleCondition aclRuleCondition = getAclRuleCondition(aclRuleModificationJson);

        if (actionType == null || aclRuleCondition == null) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Properties: Action and Condition are required").addConstraintViolation();
        }
        return valid;
    }

    private static AclRuleCondition getAclRuleCondition(AclRuleModificationJson value) {
        return value.getCondition().orElse(null);
    }

    public static Context getForwardMirrorInterfaceContext(AclRuleModificationJson value) {
        return value.getForwardMirrorInterface().orElse(null);
    }

    public static ActionType getActionType(AclRuleModificationJson value) {
        return value.getActionType().orElse(null);
    }

    public static MirrorType getMirrorType(AclRuleModificationJson value) {
        return value.getMirrorType().orElse(null);
    }

    public static Set<Context> getMirrorPortRegions(AclRuleModificationJson value) {
        return value.getMirrorPortRegions().orElse(null);
    }

    public static boolean refAssignedAndNotNull(Ref ref) {
        return ref.isAssigned() && ref.get() != null;
    }
}
