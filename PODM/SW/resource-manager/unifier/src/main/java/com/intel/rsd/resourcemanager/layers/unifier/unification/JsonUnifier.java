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

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactoryImpl.UnificationMappings;
import com.intel.rsd.resourcemanager.layers.unifier.unification.converters.CompositeConverter;
import lombok.extern.slf4j.Slf4j;
import org.springframework.stereotype.Component;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Optional;

import static com.intel.rsd.resourcemanager.layers.unifier.unification.OdataIdMapper.createDeunificationMapper;
import static com.intel.rsd.resourcemanager.layers.unifier.unification.OdataIdMapper.createUnificationMapper;

@Slf4j
@Component
public final class JsonUnifier implements Unifier<JsonNode>, Deunifier<JsonNode> {
    private final CompositeConverter compositeConverter = new CompositeConverter();

    @Override
    public JsonNode unify(JsonNode jsonNode, UnifiedOdataIdResolver resolver) {
        if (jsonNode == null) {
            return null;
        }
        ErrorsCollectingResolver errorsGatheringResolver = new ErrorsCollectingResolver(jsonNode, resolver);
        try {
            return compositeConverter.convert(null, jsonNode, createUnificationMapper(resolver));
        } finally {
            errorsGatheringResolver.logErrors();
        }
    }

    @Override
    public JsonNode deunify(JsonNode toBeDeunified, UnifiedOdataIdResolver resolver) {
        if (toBeDeunified == null) {
            return null;
        }
        return compositeConverter.convert(null, toBeDeunified.deepCopy(), createDeunificationMapper(resolver));
    }

    @Override
    public Class<JsonNode> getDeunifiableClass() {
        return JsonNode.class;
    }

    @Override
    public Class<JsonNode> getUnifiableClass() {
        return JsonNode.class;
    }

    class ErrorsCollectingResolver implements UnifiedOdataIdResolver {
        private JsonNode jsonNode;
        private UnifiedOdataIdResolver delegate;
        private List<String> unresolvedKeys = new ArrayList<>();

        ErrorsCollectingResolver(JsonNode jsonNode, UnifiedOdataIdResolver delegate) {
            this.jsonNode = jsonNode;
            this.delegate = delegate;
        }

        @Override
        public Optional<String> toUnified(String serviceSpecificOdataId) {
            Optional<String> unified = delegate.toUnified(serviceSpecificOdataId);
            if (!unified.isPresent()) {
                unresolvedKeys.add(serviceSpecificOdataId);
            }
            return unified;
        }

        @Override
        public Optional<String> toServiceSpecific(String unifiedOdataId) {
            return delegate.toServiceSpecific(unifiedOdataId);
        }

        @Override
        public void updateMapping(Collection<Resource> resources) {
            delegate.updateMapping(resources);
        }

        @Override
        public UnificationMappings getUnificationMappings() {
            return delegate.getUnificationMappings();
        }

        @Override
        public boolean updateMapping(Resource resource) {
            return delegate.updateMapping(resource);
        }

        void logErrors() {
            try {
                if (!unresolvedKeys.isEmpty()) {
                    log.error(
                        "Unresolved URIs detected after {} unification: {}",
                        jsonNode.findValue("@odata.id").asText("UNKNOWN TYPE"),
                        unresolvedKeys
                    );
                }
            } finally {
                unresolvedKeys.clear();
            }
        }
    }
}
