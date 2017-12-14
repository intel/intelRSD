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

import com.intel.podm.redfish.json.templates.actions.ComposedNodePartialRepresentation;

import javax.validation.Constraint;
import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import javax.validation.Payload;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;
import java.util.Objects;

import static java.lang.annotation.ElementType.PARAMETER;
import static java.lang.annotation.RetentionPolicy.RUNTIME;
import static java.util.Optional.ofNullable;

@Constraint(validatedBy = ComposedNodeConstraint.ComposedNodeConstraintValidator.class)
@Target(PARAMETER)
@Retention(RUNTIME)
public @interface ComposedNodeConstraint {

    String message() default "Field boot cannot be empty or null.";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

    class ComposedNodeConstraintValidator implements ConstraintValidator<ComposedNodeConstraint, ComposedNodePartialRepresentation> {
        @Override
        public void initialize(ComposedNodeConstraint composedNodeConstraintAnnotation) {
        }

        @Override
        public boolean isValid(ComposedNodePartialRepresentation computerSystemPartialRepresentation, ConstraintValidatorContext context) {
            return ofNullable(computerSystemPartialRepresentation)
                .map(json -> json.boot)
                .filter(Objects::nonNull)
                .filter(boot -> boot.getBootSourceOverrideTarget() != null || boot.getBootSourceOverrideEnabled() != null
                    || boot.getBootSourceOverrideMode() != null)
                .isPresent();
        }
    }
}


