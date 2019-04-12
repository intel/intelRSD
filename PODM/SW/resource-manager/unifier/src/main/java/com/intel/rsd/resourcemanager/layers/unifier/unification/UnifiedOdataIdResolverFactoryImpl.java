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

import com.intel.rsd.crawler.Crawler;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.KnownPatterns;
import com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers.Transformers;
import lombok.Getter;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Map;

@Component
public class UnifiedOdataIdResolverFactoryImpl implements UnifiedOdataIdResolverFactory {
    private final KnownPatterns knownPatterns;
    private final UnifierMappingsProvider unifierMappingsProvider;
    private Transformers transformers;

    @Autowired
    UnifiedOdataIdResolverFactoryImpl(KnownPatterns knownPatterns,
                                      UnifierMappingsProvider unifierMappingsProvider,
                                      Transformers transformers) {

        this.knownPatterns = knownPatterns;
        this.unifierMappingsProvider = unifierMappingsProvider;
        this.transformers = transformers;
    }

    @Override
    public CacheBasedResolver createUnifiedOdataIdResolver(String serviceId) {
        val mappings = new UnificationMappings(serviceId, unifierMappingsProvider);
        return new CacheBasedResolver(mappings, knownPatterns, transformers);
    }

    @Override
    public UnifiedOdataIdResolver createLoadingResolver(String id, Crawler crawler) {
        return new LoadingResolver(crawler, createUnifiedOdataIdResolver(id));
    }

    @Getter
    public static class UnificationMappings {
        private final Map<String, String> mapping;
        private final Map<String, String> reverseMapping;
        private final Map<String, String> constantsMapping;

        public UnificationMappings(String serviceId, UnifierMappingsProvider unifierMappingsProvider) {
            this.mapping = unifierMappingsProvider.getMappingMap(getClass().getName(), serviceId);
            this.reverseMapping = unifierMappingsProvider.getReverseMappingMap(getClass().getName(), serviceId);
            this.constantsMapping = unifierMappingsProvider.getConstantsMappingMap(getClass().getName(), serviceId);
        }

        public UnificationMappings(Map<String, String> mapping, Map<String, String> reverseMapping, Map<String, String> constantsMapping) {
            this.mapping = mapping;
            this.reverseMapping = reverseMapping;
            this.constantsMapping = constantsMapping;
        }
    }
}
