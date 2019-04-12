/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.rest.redfish.json.actions;

import javax.validation.Constraint;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import javax.validation.Payload;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;

import static java.lang.annotation.ElementType.PARAMETER;
import static java.lang.annotation.RetentionPolicy.RUNTIME;

@Constraint(validatedBy = ComposedNodePatchConstraint.ComposedNodePatchConstraintValidator.class)
@Target(PARAMETER)
@Retention(RUNTIME)
public @interface ComposedNodePatchConstraint {
    String message() default "{com.intel.rsd.nodecomposer.rest.redfish.json.actions.ComposedNodePatchConstraint.message}";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

    class ComposedNodePatchConstraintValidator implements ConstraintValidator<ComposedNodePatchConstraint, ComposedNodePartialRepresentation> {
        @Override
        public void initialize(ComposedNodePatchConstraint composedNodePatchConstraintAnnotation) {
        }

        @Override
        public boolean isValid(ComposedNodePartialRepresentation representation, ConstraintValidatorContext context) {
            if (representation.boot == null && representation.clearTpmOnDelete == null && representation.oem == null) {
                return false;
            }

            boolean isValidBoot = validateBoot(representation.boot);
            boolean isValidOem = validateOem(representation.oem);

            return isValidBoot && isValidOem;
        }

        private boolean validateOem(ComposedNodePartialRepresentation.Oem oem) {
            if (oem == null) {
                return true;
            }

            return oem.intelRackScale != null && oem.intelRackScale.taggedValues != null;
        }

        private boolean validateBoot(ComposedNodePartialRepresentation.Boot boot) {
            if (boot == null) {
                return true;
            }
            return boot.getBootSourceOverrideTarget() != null || boot.getBootSourceOverrideEnabled() != null || boot.getBootSourceOverrideMode() != null;
        }
    }
}


