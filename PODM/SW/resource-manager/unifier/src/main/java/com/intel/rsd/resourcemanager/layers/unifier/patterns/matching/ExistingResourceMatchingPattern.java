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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.matching;

import lombok.EqualsAndHashCode;
import lombok.NonNull;
import lombok.RequiredArgsConstructor;

import java.util.Collection;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.google.common.base.Joiner.on;
import static java.lang.String.format;
import static java.util.stream.Collectors.joining;

@EqualsAndHashCode
@RequiredArgsConstructor
public final class ExistingResourceMatchingPattern {
    @NonNull
    private final Collection<SegmentMatchingPattern> segments;

    private Pattern extractionPattern;

    @Override
    public String toString() {
        return format("[/%s]", on('/').join(segments));
    }

    String toRegularExpression() {
        return "\\/" + segments
            .stream()
            .map(SegmentMatchingPattern::toRegularExpression)
            .collect(joining("\\/"));
    }

    public String extractFrom(String odataId) {
        Pattern pattern = getExtractionPattern();
        Matcher matcher = pattern.matcher(odataId);
        if (matcher.find()) {
            return matcher.group();
        }
        throw new IllegalArgumentException(format("Could not extractFrom pattern: %s from odataId: %s", this, odataId));
    }

    private Pattern getExtractionPattern() {
        if (extractionPattern != null) {
            return extractionPattern;
        }

        extractionPattern = Pattern.compile("^" + toRegularExpression());
        return extractionPattern;
    }
}
