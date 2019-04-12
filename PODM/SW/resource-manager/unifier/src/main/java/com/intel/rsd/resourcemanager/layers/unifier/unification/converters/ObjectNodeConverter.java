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
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.rsd.resourcemanager.layers.unifier.unification.OdataIdMapper;

import java.util.Iterator;
import java.util.Map.Entry;
import java.util.Set;

import static com.google.common.collect.Sets.newHashSet;

class ObjectNodeConverter implements JsonNodeConverter {

    private static final String ODATA_ID_FIELD_NAME = "@odata.id";

    private final JsonNodeConverter delegate;

    private final Set<String> unifiableFields = newHashSet(ODATA_ID_FIELD_NAME, "message", "target", "OriginOfCondition");

    ObjectNodeConverter(JsonNodeConverter delegate) {
        this.delegate = delegate;
    }

    @Override
    public JsonNode convert(String propertyName, JsonNode jsonNode, OdataIdMapper mapper) {
        ObjectNode objectNode = (ObjectNode) jsonNode;
        unifyEntityId(objectNode, mapper);

        for (String field : unifiableFields) {
            if (objectNode.has(field) && objectNode.get(field) instanceof TextNode) {
                objectNode.set(field, delegate.convert(field, objectNode.get(field), mapper));
            }
        }

        Iterator<Entry<String, JsonNode>> it = objectNode.fields();

        while (it.hasNext()) {
            Entry<String, JsonNode> entry = it.next();
            JsonNode value = entry.getValue();

            if (value.isContainerNode()) {
                delegate.convert(entry.getKey(), value, mapper);
            }
        }

        return objectNode;
    }

    private void unifyEntityId(ObjectNode node, OdataIdMapper mapper) {
        if (node.has("Id") && node.has(ODATA_ID_FIELD_NAME)) {
            String odataId = node.get(ODATA_ID_FIELD_NAME).asText();
            node.set(
                "Id",
                mapper.mapEntityId(odataId).map(TextNode::new)
                    .orElseThrow(() -> new RuntimeException("Given odataId cannot be unified/deunified: " + odataId))
            );
        }
    }
}
