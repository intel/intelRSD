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
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.intel.rsd.resourcemanager.layers.unifier.unification.OdataIdMapper;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import static com.google.common.collect.Sets.newHashSet;

class JsonArrayConverter implements JsonNodeConverter {

    private final JsonNodeConverter delegate;
    private final Set<String> textNodesCollections = newHashSet("MetricProperties");

    JsonArrayConverter(JsonNodeConverter delegate) {
        this.delegate = delegate;
    }

    @Override
    public JsonNode convert(String propertyName, JsonNode jsonNode, OdataIdMapper mapper) {
        if (textNodesCollections.contains(propertyName)) {
            convertTextNodesArray(propertyName, jsonNode, mapper);
        } else {
            for (JsonNode nextNode : jsonNode) {
                if (nextNode.isContainerNode()) {
                    delegate.convert(propertyName, nextNode, mapper);
                }
            }
        }
        return jsonNode;
    }

    private void convertTextNodesArray(String propertyName, JsonNode jsonNode, OdataIdMapper mapper) {
        if (jsonNode.size() == 0) {
            return;
        }
        if (jsonNode.get(0).isTextual()) {
            List<JsonNode> convertedString = new ArrayList<>();
            for (JsonNode nextNode : jsonNode) {
                convertedString.add(delegate.convert(propertyName, nextNode, mapper));

            }
            ArrayNode arrayNode = (ArrayNode) jsonNode;
            arrayNode.removeAll();
            arrayNode.addAll(convertedString);
        }
    }
}
