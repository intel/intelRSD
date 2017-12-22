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

package com.intel.podm.redfish.json.templates.actions.constraints;

import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.ActionType;
import com.intel.podm.common.types.MirrorType;
import com.intel.podm.common.types.Ref;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleCondition;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleConditionId;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleConditionIp;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleConditionMacAddress;
import com.intel.podm.common.types.redfish.RedfishEthernetSwitchAclRule.AclRuleConditionPort;
import com.intel.podm.redfish.json.templates.actions.AclRuleModificationJson;
import com.intel.podm.redfish.json.templates.actions.AclRuleModificationJson.AclRuleConditionImpl;
import org.apache.commons.validator.routines.IntegerValidator;

import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import java.util.Set;

import static com.intel.podm.common.types.ActionType.FORWARD;
import static com.intel.podm.common.types.ActionType.MIRROR;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.checkNonNulls;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.getActionType;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.getForwardMirrorInterfaceContext;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.getMirrorPortRegions;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.getMirrorType;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.isNotEmptyRequest;
import static com.intel.podm.redfish.json.templates.actions.constraints.EthernetSwitchAclRuleConstraintValidatorHelper.refAssignedAndNotNull;
import static org.apache.commons.collections.CollectionUtils.isEmpty;

public class EthernetSwitchAclRuleConstraintValidator implements ConstraintValidator<EthernetSwitchAclRuleConstraint, AclRuleModificationJson> {

    private static final int MIN_PORT_RANGE = 0;
    private static final int MAX_PORT_RANGE = 255;

    private EthernetSwitchAclRuleConstraint constraint;

    @Override
    public void initialize(EthernetSwitchAclRuleConstraint constraintAnnotation) {
        this.constraint = constraintAnnotation;
    }

    @Override
    public boolean isValid(AclRuleModificationJson value, ConstraintValidatorContext context) {
        context.disableDefaultConstraintViolation();
        switch (constraint.actionType()) {
            case Create:
                return validateCreationRequest(value, context);
            case Update:
                return validateUpdateRequest(value, context);
            default:
                return true;
        }
    }

    private boolean validateUpdateRequest(AclRuleModificationJson value, ConstraintValidatorContext context) {
        return isNotEmptyRequest(value, context)
            && checkIfForwardMirrorInterfaceExists(value, context)
            && checkMirrorActionRequiredProperties(value, context)
            && checkConditionIfDefined(value, context);
    }

    private boolean validateCreationRequest(AclRuleModificationJson value, ConstraintValidatorContext context) {
        return checkNonNulls(value, context)
            && checkIfForwardMirrorInterfaceExists(value, context)
            && checkMirrorActionRequiredProperties(value, context)
            && checkCondition(value, context);
    }

    private boolean checkIfForwardMirrorInterfaceExists(AclRuleModificationJson value, ConstraintValidatorContext context) {
        boolean valid = true;
        ActionType actionType = getActionType(value);
        if (FORWARD.equals(actionType) || MIRROR.equals(actionType)) {
            if (getForwardMirrorInterfaceContext(value) == null) {
                valid = false;
                context.buildConstraintViolationWithTemplate("Property ForwardMirrorInterface is required for " + actionType + " action")
                    .addConstraintViolation();
            }
        }

        return valid;
    }

    private boolean checkMirrorActionRequiredProperties(AclRuleModificationJson value, ConstraintValidatorContext context) {
        boolean valid = true;

        Set<Context> mirrorPortRegion = getMirrorPortRegions(value);
        MirrorType mirrorType = getMirrorType(value);
        ActionType actionType = getActionType(value);

        if (MIRROR.equals(actionType)) {
            if (mirrorType == null || isEmpty(mirrorPortRegion)) {
                valid = false;
                context.buildConstraintViolationWithTemplate(
                    "Properties: MirrorPortRegion and MirrorType are required for " + actionType + " actionType"
                ).addConstraintViolation();
            }
        }

        return valid;
    }

    private boolean checkCondition(AclRuleModificationJson value, ConstraintValidatorContext context) {
        boolean valid;
        AclRuleCondition condition = value.getCondition().get();
        AclRuleConditionImpl conditionWithUnsetProperties = new AclRuleConditionImpl();
        if (conditionWithUnsetProperties.equals(condition)) {
            valid = false;
            context.buildConstraintViolationWithTemplate("At least one property of condition object is required").addConstraintViolation();
        } else {
            valid = checkAllFieldsInCondition(context, condition);
        }
        return valid;
    }

    @SuppressWarnings({"checkstyle:CyclomaticComplexity", "checkstyle:ExecutableStatementCount", "checkstyle:MethodLength"})
    private boolean checkAllFieldsInCondition(ConstraintValidatorContext context, AclRuleCondition condition) {
        Ref<? extends AclRuleConditionIp> ipSource = condition.getIpSource();
        boolean valid = checkParentAndChildRefExists(refAssignedAndNotNull(ipSource) && !refAssignedAndNotNull(ipSource.get().getIpv4Address()),
            "Field IPv4Address in IPSource (Condition object) is required", context);

        Ref<? extends AclRuleConditionIp> ipDestination = condition.getIpDestination();
        valid &= checkParentAndChildRefExists(refAssignedAndNotNull(ipDestination) && !refAssignedAndNotNull(ipDestination.get().getIpv4Address()),
            "Field IPv4Address in IpDestination (Condition object) is required", context);

        Ref<? extends AclRuleConditionMacAddress> macSource = condition.getMacSource();
        valid &= checkParentAndChildRefExists(refAssignedAndNotNull(macSource) && !refAssignedAndNotNull(macSource.get().getMacAddress()),
            "Field MacAddress in MacSource (Condition object) is required", context);

        Ref<? extends AclRuleConditionMacAddress> macDestination = condition.getMacDestination();
        valid &= checkParentAndChildRefExists(refAssignedAndNotNull(macDestination) && !refAssignedAndNotNull(macDestination.get().getMacAddress()),
            "Field MacAddress in MacDestination (Condition object) is required", context);

        Ref<? extends AclRuleConditionId> vlanId = condition.getVlanId();
        valid &= checkParentAndChildRefExists(refAssignedAndNotNull(vlanId) && !refAssignedAndNotNull(vlanId.get().getId()),
            "Field Id in VLANId (Condition object) is required", context);

        Ref<? extends AclRuleConditionPort> l4SourcePort = condition.getL4SourcePort();
        valid &= checkParentAndChildRefExists(refAssignedAndNotNull(l4SourcePort) && !refAssignedAndNotNull(l4SourcePort.get().getPort()),
            "Field Port in L4SourcePort (Condition object) is required", context);

        Ref<? extends AclRuleConditionPort> l4DestinationPort = condition.getL4DestinationPort();
        valid &= checkParentAndChildRefExists(refAssignedAndNotNull(l4DestinationPort) && !refAssignedAndNotNull(l4DestinationPort.get().getPort()),
            "Field Port in L4DestinationPort (Condition object) is required", context);

        Ref<Integer> l4ProtocolRef = condition.getL4Protocol();
        if (refAssignedAndNotNull(l4ProtocolRef)
            && !IntegerValidator.getInstance().isInRange(l4ProtocolRef.get(), MIN_PORT_RANGE, MAX_PORT_RANGE)) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Field L4Protocol (Condition object) is not in range <" + MIN_PORT_RANGE
                + ", " + MAX_PORT_RANGE + ">").addConstraintViolation();
        }
        return valid;
    }

    private boolean checkParentAndChildRefExists(Boolean referenceNotNull, String messageTemplate, ConstraintValidatorContext context) {
        if (referenceNotNull) {
            context.buildConstraintViolationWithTemplate(messageTemplate).addConstraintViolation();
            return false;
        }
        return true;
    }

    private boolean checkConditionIfDefined(AclRuleModificationJson value, ConstraintValidatorContext context) {
        AclRuleCondition condition = value.getCondition().orElse(null);

        return condition == null || checkCondition(value, context);
    }
}
