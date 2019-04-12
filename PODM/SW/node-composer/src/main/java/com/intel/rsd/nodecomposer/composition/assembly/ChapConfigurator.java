/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.composition.assembly;

import com.intel.rsd.nodecomposer.types.Chap;
import com.intel.rsd.nodecomposer.types.Ref;
import org.apache.commons.text.RandomStringGenerator;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.annotation.PostConstruct;
import java.security.SecureRandom;

import static org.apache.commons.text.CharacterPredicates.DIGITS;
import static org.apache.commons.text.CharacterPredicates.LETTERS;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class ChapConfigurator {
    private final int usernameLength;
    private final int secretLength;

    private RandomStringGenerator randomStringGenerator;

    public ChapConfigurator(@Value("${iscsi-chap.username-length:128}") int usernameLength,
                            @Value("${iscsi-chap.secret-length:128}") int secretLength) {
        this.usernameLength = usernameLength;
        this.secretLength = secretLength;
    }

    @PostConstruct
    public void init() {
        randomStringGenerator = new RandomStringGenerator.Builder()
            .usingRandom(new SecureRandom()::nextInt)
            .withinRange('0', 'z')
            .filteredBy(LETTERS, DIGITS)
            .build();
    }

    public Chap generateChap() {
        Chap chap = new Chap();

        chap.setUsername(Ref.of(generateUsername()));
        chap.setSecret(Ref.of(generateSecret()));
        chap.setMutualUsername(Ref.of(generateUsername()));
        chap.setMutualSecret(Ref.of(generateSecret()));

        return chap;
    }

    public String generateUsername() {
        return randomStringGenerator.generate(usernameLength);
    }

    public String generateSecret() {
        return randomStringGenerator.generate(secretLength);
    }
}
