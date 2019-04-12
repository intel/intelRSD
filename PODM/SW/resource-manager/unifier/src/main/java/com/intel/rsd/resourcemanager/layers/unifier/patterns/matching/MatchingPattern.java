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
import lombok.Getter;
import lombok.NonNull;
import lombok.RequiredArgsConstructor;
import lombok.val;
import org.springframework.web.util.UriComponentsBuilder;

import java.net.URI;
import java.util.Collection;
import java.util.regex.Pattern;

import static com.google.common.base.Joiner.on;
import static java.util.stream.Collectors.joining;
import static org.apache.commons.lang3.StringUtils.chop;

@EqualsAndHashCode
@Getter
@RequiredArgsConstructor
public final class MatchingPattern {
    private final ExistingResourceMatchingPattern existingResource;
    @NonNull
    private final Collection<SegmentMatchingPattern> segments;

    private Pattern compiledRegularExpression;

    public boolean matches(@NonNull String odataId) {
        String odataIdInCanonicalForm = canonicalizeOdataId(odataId);

        Pattern pattern = toRegularExpression();
        return pattern
            .matcher(odataIdInCanonicalForm)
            .find();
    }

    private String canonicalizeOdataId(String odataId) {
        String path = URI.create(odataId).getPath();

        if (path.endsWith("/")) {
            path = chop(path);
        }

        return UriComponentsBuilder.fromUriString(odataId).replacePath(path).toUriString();
    }

    private Pattern toRegularExpression() {
        if (compiledRegularExpression != null) {
            return compiledRegularExpression;
        }

        val builder = new StringBuilder("^");
        if (existingResource != null) {
            builder.append(existingResource.toRegularExpression());
        }

        if (!segments.isEmpty()) {
            builder.append("\\/");
        }

        val joinedSegments = segments
            .stream()
            .map(SegmentMatchingPattern::toRegularExpression)
            .collect(joining("\\/"));

        builder.append(joinedSegments);
        builder.append("$");
        compiledRegularExpression = Pattern.compile(builder.toString());
        return compiledRegularExpression;
    }

    @Override
    public String toString() {
        return (existingResource == null ? "" : existingResource) + "/" + on('/').join(segments);
    }
}
