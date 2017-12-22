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

import com.intel.podm.common.types.redfish.RedfishComputerSystem.Boot;
import com.intel.podm.common.types.redfish.RedfishComputerSystem.TrustedModule;
import com.intel.podm.redfish.json.templates.actions.ComputerSystemPartialRepresentation;

import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import java.util.Set;

import static com.google.common.collect.Iterables.isEmpty;

@SuppressWarnings({"checkstyle:CyclomaticComplexity", "checkstyle:BooleanExpressionComplexity", "checkstyle:ExecutableStatementCount",
    "checkstyle:NPathComplexity"})
public class ComputerSystemConstraintValidator implements ConstraintValidator<ComputerSystemConstraint, ComputerSystemPartialRepresentation> {

    private ComputerSystemConstraint constraint;

    @Override
    public void initialize(ComputerSystemConstraint constraintAnnotation) {
        this.constraint = constraintAnnotation;
    }

    @Override
    public boolean isValid(ComputerSystemPartialRepresentation computerSystemPartialRepresentation, ConstraintValidatorContext context) {
        if (computerSystemPartialRepresentation == null) {
            return false;
        }

        return isAssetTagNull(computerSystemPartialRepresentation.getAssetTag(), context)
            || isBootObjectNull(computerSystemPartialRepresentation.getBoot(), context)
            || isUserModeEnabledNull(computerSystemPartialRepresentation.getUserModeEnabled(), context)
            || areTpmTrustedModulesNotSet(computerSystemPartialRepresentation.getTrustedModules(), context);
    }

    private boolean isBootObjectNull(Boot bootJson, ConstraintValidatorContext context) {
        boolean valid = true;
        if (bootJson == null || bootPropertiesAreNull(bootJson)) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Property: Boot source override properties are empty.").addConstraintViolation();
        }
        return valid;
    }

    private boolean bootPropertiesAreNull(Boot bootJson) {
        return bootJson.getBootSourceOverrideTarget() == null
            && bootJson.getBootSourceOverrideEnabled() == null
            && bootJson.getBootSourceOverrideMode() == null;
    }

    private boolean isAssetTagNull(String assetTag, ConstraintValidatorContext context) {
        boolean valid = true;
        if (assetTag == null) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Property: AssetTag is empty.").addConstraintViolation();
        }
        return valid;
    }

    private boolean isUserModeEnabledNull(Boolean userModeEnabled, ConstraintValidatorContext context) {
        boolean valid = true;
        if (userModeEnabled == null) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Property: UserModeEnabled is empty.").addConstraintViolation();
        }
        return valid;
    }

    private boolean areTpmTrustedModulesNotSet(Set<TrustedModule> trustedModules, ConstraintValidatorContext context) {
        boolean valid = true;
        if (trustedModules == null || isEmpty(trustedModules) || areAllTrustedModulePropertiesNull(trustedModules)) {
            valid = false;
            context.buildConstraintViolationWithTemplate("Property: TrustedModules list contains empty elements.").addConstraintViolation();
        }
        return valid;
    }

    private boolean areAllTrustedModulePropertiesNull(Set<TrustedModule> trustedModules) {
        return trustedModules.stream().allMatch(trustedModule -> trustedModule.getInterfaceType() == null
            && trustedModule.getFirmwareVersion() == null
            && trustedModule.getStatus() == null);
    }
}
