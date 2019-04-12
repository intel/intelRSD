/*
 * Copyright (c) 2019 Intel Corporation
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
import org.springframework.stereotype.Component;

import java.util.Objects;
import java.util.stream.Stream;

@Component
public class FirstNonEmptyFunction implements TransformingFunction {

    @Override
    public String functionId() {
        return "firstNonEmpty";
    }

    @Override
    public String invoke(JsonNode... valueNodes) {
        return Stream.of(valueNodes)
            .filter(Objects::nonNull)
            .filter(JsonNode::isValueNode)
            .filter(valueNode -> !valueNode.isNull())
            .filter(valueNode -> !valueNode.asText().isEmpty())
            .findFirst()
            .orElseThrow(() -> new RuntimeException("Cannot determine first non null value node, all of provided values are null or empty"))
            .asText();
    }
}
