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

import com.intel.podm.redfish.json.templates.actions.CreateStorageServiceVolumeJson;
import com.intel.podm.redfish.json.templates.actions.constraints.Validator.ValidationRule;

import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;

import static java.util.Objects.isNull;

public class VolumeCreationConstraintValidator implements ConstraintValidator<VolumeCreationConstraint, CreateStorageServiceVolumeJson> {
    @Override
    public void initialize(VolumeCreationConstraint constraintAnnotation) {
    }

    @Override
    public boolean isValid(CreateStorageServiceVolumeJson value, ConstraintValidatorContext context) {
        context.disableDefaultConstraintViolation();
        return Validator.<CreateStorageServiceVolumeJson>instance(context)
            // TODO: add validation of ProvidingPools in CapacitySources
            .addRule(dto -> value != null, "Volume definition is required")
            .addRule(capacityBytesOccurenceValidator())
            .addRule(capacityBytesValueValidator())
            .isValid(value);
    }

    private ValidationRule<CreateStorageServiceVolumeJson> capacityBytesOccurenceValidator() {
        return new ValidationRule<>(request -> !isNull(request.getCapacityBytes()), "CapacityBytes value is required.");
    }

    private ValidationRule<CreateStorageServiceVolumeJson> capacityBytesValueValidator() {
        return new ValidationRule<>(request -> request.getCapacityBytes().doubleValue() > 0, "Requested CapacityBytes value should be > 0.");
    }
}
