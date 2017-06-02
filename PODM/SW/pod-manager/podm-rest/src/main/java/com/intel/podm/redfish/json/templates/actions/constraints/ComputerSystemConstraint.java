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

package com.intel.podm.redfish.json.templates.actions.constraints;

import com.intel.podm.common.types.redfish.RedfishComputerSystem;

import javax.validation.Constraint;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import javax.validation.Payload;
import java.lang.annotation.ElementType;
import java.lang.annotation.Retention;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Target;

@Constraint(validatedBy = ComputerSystemConstraint.ComputerSystemConstraintValidator.class)
@Target(ElementType.PARAMETER)
@Retention(RetentionPolicy.RUNTIME)
public @interface ComputerSystemConstraint {

    String message() default "Cannot update Computer System with empty values";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

    class ComputerSystemConstraintValidator implements ConstraintValidator<ComputerSystemConstraint, RedfishComputerSystem> {
        @Override
        public void initialize(ComputerSystemConstraint computerSystemConstraintAnnotation) {
        }

        @Override
        public boolean isValid(RedfishComputerSystem redfishComputerSystem, ConstraintValidatorContext context) {
            if (redfishComputerSystem == null) {
                return false;
            }

            RedfishComputerSystem.Boot bootJson = redfishComputerSystem.getBoot();

            if (redfishComputerSystem.getAssetTag() == null && bootJson == null) {
                return false;
            }

            return !checkIfBootObjectHasOnlyEmptyIfNecessary(redfishComputerSystem, bootJson);
        }

        private boolean checkIfBootObjectHasOnlyEmptyIfNecessary(
                RedfishComputerSystem computerSystemPartialRepresentation,
                RedfishComputerSystem.Boot bootJson) {
            if (computerSystemPartialRepresentation.getAssetTag() == null && bootJson != null) {
                if (bootJson.getBootSourceOverrideTarget() == null
                        && bootJson.getBootSourceOverrideEnabled() == null
                        && bootJson.getBootSourceOverrideMode() == null) {
                    return true;
                }
            }
            return false;
        }
    }
}
