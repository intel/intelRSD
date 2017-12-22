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

import com.intel.podm.redfish.json.templates.actions.UpdateVlanJson;

import javax.validation.Constraint;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import javax.validation.Payload;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;

import static java.lang.annotation.ElementType.PARAMETER;
import static java.lang.annotation.RetentionPolicy.RUNTIME;

@Constraint(validatedBy = VlanConstraint.VlanConstraintValidator.class)
@Target(PARAMETER)
@Retention(RUNTIME)
public @interface VlanConstraint {
    String message() default "The correct range for VLANId is between 1 and 4094.";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

    class VlanConstraintValidator implements ConstraintValidator<VlanConstraint, UpdateVlanJson> {

        private static final Integer MIN_RANGE_VLANID = 1;
        private static final Integer MAX_RANGE_VLANID = 4096;

        @Override
        public void initialize(VlanConstraint constraintAnnotation) {
        }

        @Override
        public boolean isValid(UpdateVlanJson json, ConstraintValidatorContext context) {
            Integer vlanId = json.getVlanId();
            return vlanId != null && vlanId >= MIN_RANGE_VLANID && vlanId <= MAX_RANGE_VLANID;
        }
    }
}
