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

package com.intel.podm.common.types.redfish;

import java.lang.annotation.Retention;
import java.lang.annotation.Target;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import static java.lang.annotation.ElementType.TYPE;
import static java.lang.annotation.RetentionPolicy.RUNTIME;
import static java.util.Arrays.asList;

@Retention(RUNTIME)
@Target(TYPE)
public @interface OemType {
    String OEM_ELEMENT_SEPARATOR = "->";

    Set<String> KNOWN_OEMS = new HashSet<String>() {{
        add("Intel_RackScale");
    }};

    Type value();

    enum Type {
        TOP_LEVEL_OEM("Oem"),
        OEM_IN_LINKS("Links", "Oem"),
        OEM_IN_ACTIONS("Actions", "Oem");

        private final List<String> path = new ArrayList<>();

        Type(String... values) {
            path.addAll(asList(values));
        }

        public List<String> getPath() {
            return path;
        }
    }
}
