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

import javax.validation.ConstraintValidatorContext;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.function.Predicate;

public final class Validator<T> {
    private ConstraintValidatorContext context;
    private List<ValidationRule<T>> rulesInOrder = new ArrayList<>();

    private Validator(ConstraintValidatorContext context) {
        this.context = context;
    }

    public static <T> Validator<T> instance(ConstraintValidatorContext context) {
        return new Validator<>(context);
    }

    public Validator<T> addRule(Predicate<T> predicate, String errMessage) {
        rulesInOrder.add(new ValidationRule<>(predicate, errMessage));
        return this;
    }

    public Validator<T> addRule(ValidationRule<T> validationRule) {
        rulesInOrder.add(validationRule);
        return this;
    }

    public boolean isValid(T validatingValue) {
        Optional<ValidationRule<T>> brokenRule = rulesInOrder.stream().filter(rule -> !rule.isValid(validatingValue)).findFirst();
        brokenRule.ifPresent(rule -> context.buildConstraintViolationWithTemplate(rule.errMessage).addConstraintViolation());
        return !brokenRule.isPresent();
    }

    public static class ValidationRule<T> {
        private Predicate<T> predicate;
        private String errMessage;

        ValidationRule(Predicate<T> predicate, String errMessage) {
            this.predicate = predicate;
            this.errMessage = errMessage;
        }

        boolean isValid(T validatingValue) {
            return predicate.test(validatingValue);
        }
    }
}
