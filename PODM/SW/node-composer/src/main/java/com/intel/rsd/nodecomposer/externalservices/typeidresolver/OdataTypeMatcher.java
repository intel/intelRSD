/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.externalservices.typeidresolver;

import lombok.extern.slf4j.Slf4j;

import java.util.Objects;
import java.util.Optional;
import java.util.regex.Pattern;

import static java.util.Optional.empty;
import static java.util.Optional.of;

@Slf4j
public abstract class OdataTypeMatcher {
    public static OdataTypeMatcher simpleOdataTypeMatcher(String expectedOdataType, Class clazz) {
        log.debug("registering {} with type: '{}'", clazz, expectedOdataType);
        return new OdataTypeMatcher() {
            @Override
            public Optional<Class> match(String odataType) {
                return Objects.equals(expectedOdataType, odataType)
                    ? of(clazz)
                    : empty();
            }
        };
    }

    public static OdataTypeMatcher odataTypePatternMatcher(String pattern, Class clazz) {
        log.debug("registering {} with pattern: '{}'", clazz, pattern);
        return new OdataTypeMatcher() {
            final Pattern regex = Pattern.compile(pattern);

            @Override
            public Optional<Class> match(String odataType) {
                return regex.matcher(odataType).matches()
                    ? of(clazz)
                    : empty();
            }
        };
    }

    public abstract Optional<Class> match(String odataType);
}
