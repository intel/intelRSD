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

import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.AbsolutePathSegment;

import java.util.Collection;
import java.util.Map;
import java.util.function.Function;

public class KnownPatterns {

    private final Collection<UnificationPattern> unificationPatterns;
    private final Map<String, AbsolutePathSegment> jsonPointers;
    private final Function<String, UnificationPattern> fallbackUnificationPatternGenerator;

    public KnownPatterns(Collection<UnificationPattern> unificationPatterns,
                         Map<String, AbsolutePathSegment> serviceScopeConstantsDefinitions,
                         Function<String, UnificationPattern> fallbackUnificationPatternGenerator) {

        this.unificationPatterns = unificationPatterns;
        this.jsonPointers = serviceScopeConstantsDefinitions;
        this.fallbackUnificationPatternGenerator = fallbackUnificationPatternGenerator;
    }

    public UnificationPattern findPattern(String odataId) {
        return unificationPatterns.stream()
            .filter(pattern -> pattern.getMatchingPattern().matches(odataId))
            .findFirst()
            .orElseGet(() -> fallbackUnificationPatternGenerator.apply(odataId));
    }

    public AbsolutePathSegment getServiceScopeConstantDefinition(String odataId) {
        return jsonPointers.get(odataId);
    }

    public boolean hasServiceScopeConstantDefinition(String odataId) {
        return jsonPointers.containsKey(odataId);
    }
}
