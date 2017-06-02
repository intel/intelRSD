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

package com.intel.podm.client.typeidresolver;

import com.intel.podm.common.logger.Logger;

import java.util.Objects;
import java.util.Optional;
import java.util.regex.Pattern;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.regex.Pattern.compile;

public abstract class OdataTypeMatcher {
    private static final Logger LOGGER = getLogger(OdataTypeMatcher.class);

    public static OdataTypeMatcher simpleOdataTypeMatcher(String expectedOdataType, Class clazz) {
        LOGGER.d("registering {} with type: '{}'", clazz, expectedOdataType);
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
        LOGGER.d("registering {} with pattern: '{}'", clazz, pattern);
        return new OdataTypeMatcher() {
            final Pattern regex = compile(pattern);

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
