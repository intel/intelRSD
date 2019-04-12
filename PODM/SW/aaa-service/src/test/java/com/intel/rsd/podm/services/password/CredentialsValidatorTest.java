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

import org.testng.annotations.BeforeMethod;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import javax.validation.ValidationException;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

public class CredentialsValidatorTest {

    private static final int MIN_PASSWORD_LENGTH = 3;
    private static final int MAX_PASSWORD_LENGTH = 15;
    private static final String PASSWORD_EXCEEDING_MAX_LENGTH = "PasswordExceedingMaxLength!@$";
    private static final String PASSWORD_TOO_SHORT = "sd";
    private static final String PASSWORD_LACKING_UPPERCASE = "sdasd!3";
    private static final String PASSWORD_LACKING_SPECIAL_CHAR = "Sdasd3";
    private static final String PASSWORD_OF_PROPER_SIZE_BUT_LACKING_CHARACTER_REQUIREMENTS = "sdasd";
    private static final String PASSWORD_WITHOUT_DIGITS = "Asdasd!";
    private static final String PASSWORD_LACKING_LOWERCASE = "ASDDASDA!3";
    private static final String PASSWORD_WITH_REPEATED_SEQUENCES = "Asdddddasd1!";
    private static final String PASSWORD_WITH_WHITESPACE = "Asda sd1!";
    private static final String VALID_PASSWORD = "Asdasd1!";
    private static final String DEFAULT_USERNAME = "admin!";

    private PasswordPolicyConfiguration configuration;
    private CredentialsValidator credentialsValidator;

    @DataProvider(name = "validPasswords")
    public static Object[] validPasswords() {
        return new String[]{VALID_PASSWORD};
    }

    @DataProvider(name = "invalidPasswords")
    public static Object[] invalidPasswords() {
        return new String[]{PASSWORD_WITHOUT_DIGITS, PASSWORD_LACKING_LOWERCASE, PASSWORD_LACKING_UPPERCASE, PASSWORD_LACKING_SPECIAL_CHAR,
            PASSWORD_OF_PROPER_SIZE_BUT_LACKING_CHARACTER_REQUIREMENTS, PASSWORD_TOO_SHORT, PASSWORD_WITH_REPEATED_SEQUENCES,
            PASSWORD_WITH_WHITESPACE, PASSWORD_EXCEEDING_MAX_LENGTH};
    }

    @BeforeMethod
    public void before() {
        configuration = mock(PasswordPolicyConfiguration.class);
        when(configuration.getLowercaseCharactersAmount()).thenReturn(1);
        when(configuration.getUppercaseCharactersAmount()).thenReturn(1);
        when(configuration.getSpecialCharactersAmount()).thenReturn(1);
        when(configuration.isNoRepeatedCharsAllowed()).thenReturn(true);
        when(configuration.isNoWhitespacesAllowed()).thenReturn(true);
        when(configuration.getDigitCharactersAmount()).thenReturn(1);
        when(configuration.getMinLength()).thenReturn(MIN_PASSWORD_LENGTH);
        when(configuration.getMaxLength()).thenReturn(MAX_PASSWORD_LENGTH);
        credentialsValidator = new CredentialsValidator(new PasswordPolicyRulesProvider(configuration));
    }


    @Test(dataProvider = "validPasswords")
    public void supportPasswordsFulfillingPasswordPolicy(String password) {
        credentialsValidator.validate(password, DEFAULT_USERNAME);
    }

    @Test(dataProvider = "invalidPasswords", expectedExceptions = ValidationException.class)
    public void rejectPasswordsBreakingPasswordPolicy(String password) {
        credentialsValidator.validate(password, DEFAULT_USERNAME);
    }
}
