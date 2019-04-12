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

package com.intel.rsd.resourcemanager.layers.unifier.patterns;

import lombok.val;

import java.util.function.Function;

import static java.lang.String.format;

public class FallbackUnificationPattern implements Function<String, UnificationPattern> {

    private UnificationPatternFactory unificationPatternFactory;

    public FallbackUnificationPattern(UnificationPatternFactory unificationPatternFactory) {
        this.unificationPatternFactory = unificationPatternFactory;
    }

    @Override
    public UnificationPattern apply(String odataId) {
        if (!odataId.startsWith("/redfish/v1")) {
            throw new RuntimeException(format("cannot find pattern for '%s'", odataId));
        }

        val rewritingPatternAsString = odataId.replace("/redfish/v1", "/redfish/v1/unresolved/{/redfish/v1#/UUID}");
        return unificationPatternFactory.create(
            format("[/redfish/v1]/{} -> %s", rewritingPatternAsString)
        );
    }
}
