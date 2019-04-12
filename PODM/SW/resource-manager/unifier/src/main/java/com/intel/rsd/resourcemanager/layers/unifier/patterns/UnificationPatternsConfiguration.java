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
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.ArgumentSubstitutionPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.PathSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.SegmentRewritingPattern;
import lombok.val;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.core.io.Resource;

import java.io.IOException;
import java.util.List;
import java.util.function.Function;
import java.util.stream.Stream;

import static java.util.Collections.singletonList;
import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;

@Configuration
public class UnificationPatternsConfiguration {

    private Resource patterns;

    public UnificationPatternsConfiguration(@Value("classpath:/patterns") Resource patterns) {
        this.patterns = patterns;
    }

    @Bean
    public KnownPatterns knownPatterns() throws IOException {
        val unificationPatternFactory = new UnificationPatternFactory();
        val unificationPatterns = unificationPatternFactory.readFromStream(patterns.getInputStream());

        val requiredServiceScopeConstantsCarryingPatterns = createRequiredServiceScopeConstantsCarryingPatterns(unificationPatternFactory);
        val allPatternsStream = Stream.concat(unificationPatterns.stream(), requiredServiceScopeConstantsCarryingPatterns.stream());
        val serviceScopeConstantsDefinitions = allPatternsStream
            .flatMap(this::getRewritingPatternSegments)
            .flatMap(select(ArgumentSubstitutionPattern.class))
            .flatMap(this::getSegmentsInOrder)
            .flatMap(select(AbsolutePathSegment.class))
            .collect(toMap(AbsolutePathSegment::getPath, identity(), (value1, value2) -> value1));

        return new KnownPatterns(
            unificationPatterns,
            serviceScopeConstantsDefinitions,
            new FallbackUnificationPattern(unificationPatternFactory)
        );
    }

    private List<UnificationPattern> createRequiredServiceScopeConstantsCarryingPatterns(UnificationPatternFactory unificationPatternFactory) {
        return singletonList(unificationPatternFactory.create("/redfish/v1 -> /{/redfish/v1#/UUID}"));
    }

    private Stream<SegmentRewritingPattern> getRewritingPatternSegments(UnificationPattern up) {
        return up.getRewritingPattern().getSegments().stream();
    }

    private Stream<PathSegment> getSegmentsInOrder(ArgumentSubstitutionPattern argumentSubstitutionPattern) {
        return argumentSubstitutionPattern.getSegmentsInOrder().stream();
    }

    private static <T, R> Function<T, Stream<R>> select(Class<R> clazz) {
        return e -> clazz.isInstance(e) ? Stream.of(clazz.cast(e)) : null;
    }

}
