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
import com.fasterxml.jackson.databind.node.JsonNodeType;
import lombok.val;
import org.springframework.validation.Errors;

import java.util.EnumSet;
import java.util.List;

import static com.fasterxml.jackson.databind.node.JsonNodeType.NULL;
import static com.intel.rsd.resourcemanager.layers.tagger.JsonNodeUtils.removePropertyPathRecursively;
import static com.intel.rsd.resourcemanager.layers.tagger.ReadOnlyTagsActions.rejectIfContainsReadOnlyTags;
import static com.intel.rsd.resourcemanager.layers.tagger.TagBundle.newTagBundle;
import static java.lang.String.format;

class TagsExtractor {
    private final List<TagDefinition> tagDefinitions;

    TagsExtractor(List<TagDefinition> tagDefinitions) {
        this.tagDefinitions = tagDefinitions;
    }

    /**
     * Modifies passed JsonNode so that it no longer contains elements specified by TagDefinitions.
     * Removed elements are returned in a TagBundle with JsonPointer pointing to their original location.
     * If type of any element did not match the type specified in its respective TagDefinition then the Errors object is populated with validation errors.
     *
     * @param jsonNode object that gets stripped of certain elements
     * @param errors container for validation errors
     * @return TagBundle containing removed elements keyed by their location in original object
     */
    TagBundle extractFrom(JsonNode jsonNode, Errors errors) {
        if (jsonNode == null || this.tagDefinitions.isEmpty()) {
            return newTagBundle();
        }
        rejectIfContainsReadOnlyTags(jsonNode, errors);

        val extractedTags = newTagBundle();
        for (TagDefinition tagDefinition : this.tagDefinitions) {
            val jsonPointer = tagDefinition.getProperty();
            val extractedNode = extractNodeByJsonPointer(jsonNode, jsonPointer);
            if (extractedNode != null) {
                val expectedJsonType = tagDefinition.getType();
                val encounteredJsonType = extractedNode.getNodeType();
                if (!isAcceptableJsonNodeType(encounteredJsonType, expectedJsonType)) {
                    String field = jsonPointer.toString();
                    errors.rejectValue(
                        field, "InvalidJsonNodeType",
                        format("Invalid JSON type for property '#%s': expected: '%s' encountered: '%s'", field, expectedJsonType, encounteredJsonType)
                    );
                }
                extractedTags.put(jsonPointer, extractedNode);
            }
        }
        return extractedTags;
    }

    private JsonNode extractNodeByJsonPointer(JsonNode jsonNode, JsonPointer jsonPointer) {
        val node = jsonNode.at(jsonPointer);
        if (node.isMissingNode()) {
            return null;
        }
        return removePropertyPathRecursively(jsonNode, jsonPointer, true);
    }

    private boolean isAcceptableJsonNodeType(JsonNodeType encounteredJsonType, JsonNodeType expectedJsonType) {
        return EnumSet.of(NULL, expectedJsonType).contains(encounteredJsonType);
    }
}
