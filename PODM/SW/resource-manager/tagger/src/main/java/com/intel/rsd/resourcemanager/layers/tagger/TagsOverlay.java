/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.tagger;

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.JsonNodeType;
import com.fasterxml.jackson.databind.node.TextNode;
import lombok.experimental.UtilityClass;
import lombok.extern.slf4j.Slf4j;
import lombok.val;

import java.io.IOException;
import java.util.List;

import static com.intel.rsd.resourcemanager.layers.tagger.JsonNodeUtils.putNodeAtPointer;
import static com.intel.rsd.resourcemanager.layers.tagger.ReadOnlyTagsActions.addODataTypeIfNeeded;

@UtilityClass
@Slf4j
class TagsOverlay {
    private static final ObjectMapper MAPPER = new ObjectMapper();

    /**
     * Modifies passed JsonNode so that it contains tagBundle entries placed at locations specified by TagDefinitions.
     *
     * @param jsonNode object that gets modified
     * @param tagDefinitions contains definitions of elements that will be placed in JsonNode
     * @param tagBundle contains entries that will be placed in jsonNode
     */
    static void putTagsInJsonNode(JsonNode jsonNode, List<TagDefinition> tagDefinitions, TagBundle tagBundle) {
        if (jsonNode == null) {
            return;
        }

        for (TagDefinition tagDefinition : tagDefinitions) {
            val jsonPointer = tagDefinition.getProperty();
            val value = tagBundle.get(jsonPointer);
            if (value != null) {
                putNodeAtPointer(jsonNode, jsonPointer, value);
            } else {
                putDefaultTagInJsonNode(jsonNode, tagDefinition, jsonPointer);
            }
            addODataTypeIfNeeded(tagDefinition, jsonNode);
        }
    }

    static void putDefaultTagInJsonNode(JsonNode jsonNode, TagDefinition tagDefinition, JsonPointer jsonPointer) {
        if (tagDefinition.getDefaultValue() == null) {
            putNodeAtPointer(jsonNode, jsonPointer, JsonNodeFactory.instance.nullNode());
            return;
        }
        //Jackson expects String that starts and ends with escaped sequence of '\"' , so to avoid unnecessary
        //processing explicit call of TextNode is required
        if (tagDefinition.getType().equals(JsonNodeType.STRING)) {
            putNodeAtPointer(jsonNode, jsonPointer, new TextNode(tagDefinition.getDefaultValue()));
        } else {
            JsonNode createdNode;
            try {
                createdNode = MAPPER.readTree(tagDefinition.getDefaultValue());
            } catch (IOException e) {
                log.warn("Failed to convert {} into {}. Check syntax for {} section in configuration. Changing it to NULL.",
                    tagDefinition.getDefaultValue(), tagDefinition.getType(), tagDefinition.getProperty());
                createdNode = JsonNodeFactory.instance.nullNode();
            }
            putNodeAtPointer(jsonNode, jsonPointer, createdNode);
        }
    }
}
