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

package com.intel.rsd.resourcemanager.layers.unifier.unification.converters;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.JsonNodeType;
import com.intel.rsd.resourcemanager.layers.unifier.unification.OdataIdMapper;

import java.util.HashMap;
import java.util.Map;

import static com.fasterxml.jackson.databind.node.JsonNodeType.ARRAY;
import static com.fasterxml.jackson.databind.node.JsonNodeType.OBJECT;
import static com.fasterxml.jackson.databind.node.JsonNodeType.STRING;

public final class CompositeConverter implements JsonNodeConverter {

    private final JsonNodeConverter unknownStrategy = (propertyName, jsonNode, resolver) -> {
        throw new RuntimeException("Cannot convert " + jsonNode.getNodeType());
    };

    private Map<JsonNodeType, JsonNodeConverter> strategyByNodeType;

    public CompositeConverter() {
        this.strategyByNodeType = new HashMap<JsonNodeType, JsonNodeConverter>() {
            @Override
            public JsonNodeConverter get(Object o) {
                return super.getOrDefault(o, unknownStrategy);
            }
        };
        this.strategyByNodeType.put(OBJECT, new ObjectNodeConverter(this));
        this.strategyByNodeType.put(ARRAY, new JsonArrayConverter(this));
        this.strategyByNodeType.put(STRING, new TextNodeConverter());
    }

    @Override
    public JsonNode convert(String propertyName, JsonNode jsonNode, OdataIdMapper mapper) {
        return strategyByNodeType.get(jsonNode.getNodeType()).convert(propertyName, jsonNode, mapper);
    }
}
