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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.Transformers;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactoryImpl.UnificationMappings;
import lombok.val;

import java.util.ArrayList;
import java.util.Optional;

import static java.util.Optional.empty;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.joining;

@SuppressWarnings({"checkstyle:ClassFanOutComplexity"})
public class SegmentRewritingPatternVisitor {
    private final UnificationMappings mappings;
    private Transformers transformers;

    public SegmentRewritingPatternVisitor(UnificationMappings mappings, Transformers transformers) {
        this.mappings = mappings;
        this.transformers = transformers;
    }

    /**
     * @return Optional.empty() if any of passed patterns cannot be resolved, otherwise string representation of resolved patterns.
     */
    public Optional<String> visit(Resource resource, ArgumentSubstitutionPattern segmentRewritingPattern) {
        val resolvedPathSegments = new ArrayList<Optional<String>>();
        for (PathSegment pathSegment : segmentRewritingPattern.getSegmentsInOrder()) {
            val pathSegmentResolution = pathSegment.accept(resource, this);
            if (!pathSegmentResolution.isPresent()) {
                return empty();
            }
            resolvedPathSegments.add(pathSegmentResolution);
        }

        val result = resolvedPathSegments.stream().map(Optional::get).collect(joining());
        return ofNullable(result);
    }

    public Optional<String> visit(Resource resource, ConstSegmentRewritingPattern segmentRewritingPattern) {
        return of(segmentRewritingPattern.getValue());
    }

    public Optional<String> visit(Resource resource, RelativePathSegment segment) {
        if (resource == null) {
            return empty();
        }

        return of(resource.get(segment.getPath()).asText().replace(" ", "+"));
    }

    public Optional<String> visit(Resource resource, AbsolutePathSegment segment) {
        val result = mappings.getConstantsMapping().get(segment.toString());
        return ofNullable(result);
    }

    public Optional<String> visit(Resource resource, ConstantPathSegment segment) {
        val result = segment.getConstant();
        return ofNullable(result);
    }

    public Optional<String> visit(Resource resource, CalculableSegment calculableSegment) {
        if (resource == null) {
            return empty();
        }
        val functionName = calculableSegment.getFunctionName();
        val jsonPointers = calculableSegment.getJsonPointers();
        val functionArgs = jsonPointers.stream().map(resource::get).toArray(JsonNode[]::new);
        val result = transformers.findTransformingFunction(functionName).invoke(functionArgs);
        return ofNullable(result);
    }
}
