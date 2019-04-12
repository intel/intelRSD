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

import com.intel.rsd.crawler.Resource;
import lombok.AllArgsConstructor;
import lombok.EqualsAndHashCode;
import lombok.Getter;

import java.util.List;
import java.util.Optional;

import static java.lang.String.format;
import static java.util.stream.Collectors.joining;

@EqualsAndHashCode(callSuper = false)
@Getter
@AllArgsConstructor
public final class ArgumentSubstitutionPattern extends SegmentRewritingPattern {
    private final List<PathSegment> segmentsInOrder;

    @Override
    public String toString() {
        return format("{%s}", segmentsInOrder.stream().map(Object::toString).collect(joining(" ")));
    }

    @Override
    public Optional<String> accept(Resource resource, SegmentRewritingPatternVisitor visitor) {
        return visitor.visit(resource, this);
    }
}
