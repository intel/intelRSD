/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.services.password;

import lombok.NonNull;
import org.passay.PasswordData;
import org.passay.PasswordValidator;
import org.passay.RuleResult;
import org.passay.RuleResultDetail;
import org.springframework.stereotype.Component;

import javax.validation.ValidationException;
import javax.validation.constraints.NotBlank;

import static java.util.stream.Collectors.joining;

@Component
public class CredentialsValidator {

    private final PasswordValidator passwordValidator;

    CredentialsValidator(PasswordPolicyRulesProvider passwordRules) {
        this.passwordValidator = new PasswordValidator(passwordRules.getRules());
    }

    public void validate(@NotBlank String password, @NonNull String userName) {
        PasswordData passwordData = new PasswordData(password);
        passwordData.setUsername(userName);
        passwordData.setPassword(password);
        RuleResult validationResults = passwordValidator.validate(passwordData);

        if (!validationResults.isValid()) {
            String validationErrorDetails = validationResults.getDetails().stream().map(RuleResultDetail::toString).collect(joining(", "));
            throw new ValidationException("Password: " + validationErrorDetails);
        }
    }
}
