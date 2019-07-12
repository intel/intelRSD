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

package com.intel.rsd.resourcemanager.layers.unifier.unification;

import com.intel.rsd.crawler.Resource;
import com.intel.rsd.crawler.ResourceSupplier;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.KnownPatterns;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.UnificationPattern;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.AbsolutePathSegment;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.SegmentRewritingPatternVisitor;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.Transformers;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactoryImpl.UnificationMappings;
import lombok.val;
import org.slf4j.Logger;

import java.util.Collection;
import java.util.Optional;
import java.util.stream.Stream;

import static com.intel.rsd.crawler.ResourceSupplier.asSupplier;
import static java.util.Comparator.comparing;
import static java.util.Optional.of;
import static java.util.Optional.ofNullable;
import static java.util.stream.Collectors.joining;
import static java.util.stream.Collectors.toList;
import static java.util.stream.Stream.concat;
import static org.slf4j.LoggerFactory.getLogger;

class CacheBasedResolver implements UnifiedOdataIdResolver {
    private final KnownPatterns knownPatterns;
    private Transformers transformers;
    private final UnificationMappings mappings;
    private final Logger log = getLogger(getClass());

    CacheBasedResolver(UnificationMappings mappings,
                       KnownPatterns knownPatterns,
                       Transformers transformers) {

        this.mappings = mappings;
        this.knownPatterns = knownPatterns;
        this.transformers = transformers;
    }

    @Override
    public Optional<String> toUnified(String serviceSpecificOdataId) {
        if (!mappings.getMapping().containsKey(serviceSpecificOdataId)) {
            resolveMapping(
                knownPatterns.findPattern(serviceSpecificOdataId),
                asSupplier(serviceSpecificOdataId)
            ).ifPresent(
                unifiedOdataId -> updateMappings(serviceSpecificOdataId, unifiedOdataId)
            );
        }
        return ofNullable(mappings.getMapping().get(serviceSpecificOdataId));
    }

    @Override
    public Optional<String> toServiceSpecific(String unifiedOdataId) {
        return ofNullable(mappings.getReverseMapping().get(unifiedOdataId));
    }

    @Override
    public void updateMapping(Collection<Resource> resources) {
        resources.stream().sorted(comparing(Resource::getOdataId)).forEach(this::updateMapping);
    }

    @Override
    public UnificationMappings getUnificationMappings() {
        return mappings;
    }

    @Override
    public boolean updateMapping(Resource resource) {
        val odataId = resource.getOdataId();
        val pattern = knownPatterns.findPattern(odataId);
        val resolved = resolveMapping(pattern, asSupplier(resource));
        resolved.ifPresent(value -> {
            logMapping(resource, value);
            updateMappings(odataId, value);
        });
        tryToSetServiceScopeConstants(odataId, resource);
        return resolved.isPresent();
    }

    //TODO: remove this logging once issue with unified duplications is verified successfully.
    private void logMapping(Resource resource, String unifiedOdataId) {
        val odataId = resource.getOdataId();
        val identifiers = resource.get("/Identifiers");
        if (!identifiers.isMissingNode()) {
            String oldUnifiedOdataId = mappings.getMapping().get(odataId);
            if (oldUnifiedOdataId == null) {
                log.info("Adding new mapping: {} -> {}. Identifiers: {}", odataId, unifiedOdataId, identifiers.toString());
            } else {
                log.info("Updating mapping: {} -> {}. Identifiers: {}", odataId, unifiedOdataId, identifiers.toString());
                if (!oldUnifiedOdataId.equals(unifiedOdataId)) {
                    log.warn("Found unified duplication of {}: \n1. {}, \n2. {}.\nResource identifier: {}",
                        odataId, oldUnifiedOdataId, unifiedOdataId, identifiers.toString());
                }
            }
        }
    }

    private void tryToSetServiceScopeConstants(String odataId, Resource resource) {
        if (knownPatterns.hasServiceScopeConstantDefinition(odataId) && !mappings.getConstantsMapping().containsKey(odataId)) {
            AbsolutePathSegment definition = knownPatterns.getServiceScopeConstantDefinition(odataId);
            mappings.getConstantsMapping().put(definition.toString(), resource.get(definition.getFragment()).asText());
        }
    }

    private void updateMappings(String serviceSpecificOdataId, String unifiedOdataId) {
        val oldUnifiedOdataId = mappings.getMapping().get(serviceSpecificOdataId);

        if (oldUnifiedOdataId != null && !oldUnifiedOdataId.equals(unifiedOdataId)) {
            mappings.getReverseMapping().remove(oldUnifiedOdataId);
            log.info("Removed duplicate mapping for {}", serviceSpecificOdataId);
        }

        mappings.getMapping().put(serviceSpecificOdataId, unifiedOdataId);
        mappings.getReverseMapping().put(unifiedOdataId, serviceSpecificOdataId);
    }

    private Optional<String> resolveMapping(UnificationPattern pattern, ResourceSupplier resourceSupplier) {
        val visitor = new SegmentRewritingPatternVisitor(mappings, transformers);
        val rewritingPatternSegments = pattern.getRewritingPattern().getSegments();

        val resolvedResourcePrefixAsStream = Stream.of(tryToResolveExistingResourcePrefix(pattern, resourceSupplier.getOdataId()));
        val rewritingPatternSegmentsStream = rewritingPatternSegments.stream().map(segment -> segment.accept(resourceSupplier.get(), visitor));
        val result = concat(resolvedResourcePrefixAsStream, rewritingPatternSegmentsStream).collect(toList());

        if (result.contains(Optional.<String>empty())) {
            return Optional.empty();
        }

        return of(result.stream().map(Optional::get).collect(joining("/")));
    }

    private Optional<String> tryToResolveExistingResourcePrefix(UnificationPattern pattern, String odataId) {
        String recognizedPrefix = "";
        if (pattern.getRewritingPattern().getExistingResourceSubstitution() == null) {
            return of(recognizedPrefix);
        }

        if (pattern.getMatchingPattern().getExistingResource() == null) {
            return of(recognizedPrefix);
        }

        return toUnified(pattern.getMatchingPattern().getExistingResource().extractFrom(odataId));
    }
}
