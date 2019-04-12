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

import lombok.Getter;
import org.passay.CharacterCharacteristicsRule;
import org.passay.CharacterRule;
import org.passay.EnglishCharacterData;
import org.passay.LengthRule;
import org.passay.RepeatCharacterRegexRule;
import org.passay.Rule;
import org.passay.UsernameRule;
import org.passay.WhitespaceRule;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

@Component
class PasswordPolicyRulesProvider {

    @Getter
    private final List<Rule> rules = new ArrayList<>();

    private final PasswordPolicyConfiguration configuration;

    PasswordPolicyRulesProvider(PasswordPolicyConfiguration configuration) {
        this.configuration = configuration;

        rules.addAll(createObligatoryRules());
        rules.addAll(createGeneralRules());

        if (characterCharacteristicsRequired(configuration)) {
            List<CharacterRule> characterRules = createCharacterCharacteristicsRules(configuration);
            rules.add(new CharacterCharacteristicsRule(characterRules.size(), characterRules));
        }
    }

    private Collection<? extends Rule> createGeneralRules() {
        List<Rule> generalRules = new ArrayList<>();
        if (configuration.isCheckForUsernameInPassword()) {
            generalRules.add(new UsernameRule());
        }

        if (configuration.isNoWhitespacesAllowed()) {
            generalRules.add(new WhitespaceRule());
        }

        if (configuration.isNoRepeatedCharsAllowed()) {
            generalRules.add(new RepeatCharacterRegexRule());
        }
        return generalRules;
    }

    private Collection<? extends Rule> createObligatoryRules() {
        List<Rule> obligatoryRules = new ArrayList<>();
        obligatoryRules.add(new LengthRule(configuration.getMinLength(), configuration.getMaxLength()));
        return obligatoryRules;
    }

    private List<CharacterRule> createCharacterCharacteristicsRules(PasswordPolicyConfiguration cfg) {
        List<CharacterRule> characterRules = new ArrayList<>();
        if (cfg.getLowercaseCharactersAmount() > 0) {
            characterRules.add(new CharacterRule(EnglishCharacterData.LowerCase, cfg.getLowercaseCharactersAmount()));
        }

        if (cfg.getUppercaseCharactersAmount() > 0) {
            characterRules.add(new CharacterRule(EnglishCharacterData.UpperCase, cfg.getUppercaseCharactersAmount()));
        }

        if (cfg.getDigitCharactersAmount() > 0) {
            characterRules.add(new CharacterRule(EnglishCharacterData.Digit, cfg.getDigitCharactersAmount()));
        }
        if (cfg.getSpecialCharactersAmount() > 0) {
            characterRules.add(new CharacterRule(EnglishCharacterData.Special, cfg.getSpecialCharactersAmount()));
        }
        return characterRules;
    }

    private boolean characterCharacteristicsRequired(PasswordPolicyConfiguration cfg) {
        return cfg.getLowercaseCharactersAmount() > 0 || cfg.getUppercaseCharactersAmount() > 0
            || cfg.getDigitCharactersAmount() > 0 || cfg.getSpecialCharactersAmount() > 0;
    }
}
