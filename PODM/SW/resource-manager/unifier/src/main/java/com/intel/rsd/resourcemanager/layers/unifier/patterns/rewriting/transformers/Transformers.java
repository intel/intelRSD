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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.rewriting.transformers;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;

import static java.util.function.Function.identity;
import static java.util.stream.Collectors.toMap;

@Slf4j
@Component
public class Transformers {

    private static final TransformingFunction NULL_RETURNING_FUNCTION = new TransformingFunction() {
        @Override
        public String functionId() {
            return null;
        }

        @Override
        public String invoke(JsonNode... nodes) {
            log.warn("NullReturningFunction is returning null for call with arguments({})", (Object[]) nodes);
            return null;
        }
    };

    private final Map<String, TransformingFunction> transformingFunctions;

    @Autowired
    public Transformers(Collection<TransformingFunction> functions) {
        this.transformingFunctions = functions.stream()
            .peek(transformingFunction -> log.info("Transforming function detected: {}", transformingFunction.functionId()))
            .collect(toMap(TransformingFunction::functionId, identity()));
    }

    public TransformingFunction findTransformingFunction(String functionName) {
        return transformingFunctions.getOrDefault(functionName, NULL_RETURNING_FUNCTION);
    }

}
