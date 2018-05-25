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

import com.intel.podm.business.dto.EndpointDto;
import com.intel.podm.redfish.json.templates.actions.constraints.Validator.ValidationRule;

import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import java.util.Objects;

import static com.intel.podm.common.types.EntityRole.TARGET;

public class EndpointCreationConstraintValidator implements ConstraintValidator<EndpointCreationConstraint, EndpointDto> {

    private static final ValidationRule<EndpointDto> TRANSPORT_PROTOCOL_VALIDATOR = new ValidationRule<>(
        dto -> dto.getIpTransportDetails().stream().noneMatch(transport -> transport.getTransportProtocol() == null),
        "TransportProtocol has to be specified"
    );

    private EndpointCreationConstraint constraint;

    @Override
    public void initialize(EndpointCreationConstraint constraintAnnotation) {
        this.constraint = constraintAnnotation;
    }

    @Override
    public boolean isValid(EndpointDto value, ConstraintValidatorContext context) {
        context.disableDefaultConstraintViolation();
        return Validator.<EndpointDto>instance(context)
            .addRule(Objects::nonNull, "Endpoint definition is required")
            .addRule(emptyConnectedEntitiesValidator())
            .addRule(entityRoleValidator())
            .addRule(entityLinkValidator())
            .addRule(emptyIdentifiersValidator())
            .addRule(durableNameFormatValidator())
            .addRule(durableNameValidator())
            .addRule(TRANSPORT_PROTOCOL_VALIDATOR)
            .isValid(value);
    }

    private ValidationRule<EndpointDto> entityLinkValidator() {
        return new ValidationRule<>(
            dto -> dto.getConnectedEntities().stream()
                .noneMatch(ce -> TARGET.equals(ce.getEntityRole()) && ce.getEntityLink() == null),
            "EntityLink has to be specified"
        );
    }

    private ValidationRule<EndpointDto> durableNameValidator() {
        return new ValidationRule<>(
            dto -> dto.getIdentifiers().stream().noneMatch(i -> i.getDurableName() == null),
            "DurableName parameter is required"
        );
    }

    private ValidationRule<EndpointDto> emptyConnectedEntitiesValidator() {
        return new ValidationRule<>(
            dto -> !dto.getConnectedEntities().isEmpty(), "At least one connected entity has to be defined"
        );
    }

    private ValidationRule<EndpointDto> emptyIdentifiersValidator() {
        return new ValidationRule<>(
            dto -> !dto.getIdentifiers().isEmpty(), "At least one identifier has to be defined"
        );
    }

    private ValidationRule<EndpointDto> durableNameFormatValidator() {
        return new ValidationRule<>(
            dto -> dto.getIdentifiers().stream().noneMatch(i -> i.getDurableNameFormat() == null),
            "DurableNameFormat parameter is required"
        );
    }

    private ValidationRule<EndpointDto> entityRoleValidator() {
        return new ValidationRule<>(
            dto -> dto.getConnectedEntities().stream().noneMatch(ce -> ce.getEntityRole() == null),
            "EntityRole has to be specified"
        );
    }
}
