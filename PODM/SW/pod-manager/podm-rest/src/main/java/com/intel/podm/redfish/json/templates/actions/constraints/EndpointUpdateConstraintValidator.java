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

import com.intel.podm.redfish.json.templates.actions.UpdateEndpointJson;
import com.intel.podm.redfish.json.templates.actions.constraints.Validator.ValidationRule;

import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import java.util.Objects;

public class EndpointUpdateConstraintValidator implements ConstraintValidator<EndpointUpdateConstraint, UpdateEndpointJson> {
    private EndpointUpdateConstraint constraint;

    @Override
    public void initialize(EndpointUpdateConstraint constraintAnnotation) {
        this.constraint = constraintAnnotation;
    }

    @Override
    public boolean isValid(UpdateEndpointJson value, ConstraintValidatorContext context) {
        context.disableDefaultConstraintViolation();
        return Validator.<UpdateEndpointJson>instance(context)
            .addRule(Objects::nonNull, "Endpoint definition is required")
            .addRule(endpointOemValidator())
            .addRule(endpointAuthenticationValidator())
            .addRule(authenticationUsernameValidator())
            .addRule(authenticationPasswordValidator())
            .isValid(value);
    }

    private ValidationRule<UpdateEndpointJson> endpointOemValidator() {
        return new ValidationRule<>(
            json -> json.getOem() != null && json.getOem().getRackScaleOem() != null, "Endpoint Oem definition is required"
        );
    }

    private ValidationRule<UpdateEndpointJson> endpointAuthenticationValidator() {
        return new ValidationRule<>(
            json -> json.getOem().getRackScaleOem().getAuthentication() != null, "Endpoint Authentication definition is required"
        );
    }

    private ValidationRule<UpdateEndpointJson> authenticationUsernameValidator() {
        return new ValidationRule<>(
            json -> json.getOem().getRackScaleOem().getAuthentication().getUsername() != null, "Authentication Username has to be specified"
        );
    }

    private ValidationRule<UpdateEndpointJson> authenticationPasswordValidator() {
        return new ValidationRule<>(
            json -> json.getOem().getRackScaleOem().getAuthentication().getPassword() != null, "Authentication Password has to be specified"
        );
    }
}
