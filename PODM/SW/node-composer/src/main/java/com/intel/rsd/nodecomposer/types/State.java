/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types;

import java.util.List;

import static java.util.Arrays.asList;

public enum State implements EnumeratedType {
    ENABLED("Enabled"),
    DISABLED("Disabled"),
    UNAVAILABLE_OFFLINE("UnavailableOffline", "Offline"),
    STANDBY_OFFLINE("StandbyOffline"),
    STANDBY_SPARE("StandbySpare"),
    IN_TEST("InTest"),
    STARTING("Starting"),
    ABSENT("Absent");

    private final String value;
    private final List<String> aliases;

    State(String value, String... aliases) {
        this.value = value;
        this.aliases = asList(aliases);
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public List<String> getAliases() {
        return aliases;
    }

    @Override
    public String toString() {
        return getValue();
    }

}
