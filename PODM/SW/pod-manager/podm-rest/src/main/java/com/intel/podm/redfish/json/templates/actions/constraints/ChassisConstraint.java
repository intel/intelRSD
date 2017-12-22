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

import com.intel.podm.common.types.redfish.RedfishChassis;

import javax.validation.Constraint;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import javax.validation.Payload;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;

import static com.google.common.base.Strings.isNullOrEmpty;
import static java.lang.annotation.ElementType.PARAMETER;
import static java.lang.annotation.RetentionPolicy.RUNTIME;

@Constraint(validatedBy = ChassisConstraint.ChassisConstraintValidator.class)
@Target(PARAMETER)
@Retention(RUNTIME)
public @interface ChassisConstraint {

    String message() default "Cannot update Chassis with empty values";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

    class ChassisConstraintValidator implements ConstraintValidator<ChassisConstraint, RedfishChassis> {
        @Override
        public void initialize(ChassisConstraint constraintAnnotation) {
        }

        @Override
        public boolean isValid(RedfishChassis value, ConstraintValidatorContext context) {
            return value != null
                && (value.getAssetTag() != null
                || value.getGeoTag() != null
                || !isNullOrEmpty(value.getLocationId()));
        }
    }
}
