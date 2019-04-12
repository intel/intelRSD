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
import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.val;

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Objects;

import static com.fasterxml.jackson.databind.node.JsonNodeType.NULL;
import static com.fasterxml.jackson.databind.node.JsonNodeType.OBJECT;
import static com.intel.rsd.json.JsonUtils.stringToJsonNode;

public final class TagBundle {
    private final LinkedHashMap<JsonPointer, JsonNode> bundle;

    private TagBundle() {
        this.bundle = new LinkedHashMap<>();
    }

    private TagBundle(TagBundle source) {
        this.bundle = new LinkedHashMap<>(source.bundle);
    }

    static TagBundle newTagBundle() {
        return new TagBundle();
    }

    public static TagBundle newTagBundleFrom(Map<String, String> serializedTags) {
        TagBundle bundle = new TagBundle();
        for (Entry<String, String> entry : serializedTags.entrySet()) {
            bundle.put(JsonPointer.compile(entry.getKey()), stringToJsonNode(entry.getValue()));
        }
        return bundle;
    }

    public Map<String, String> asStringStringMap() {
        val output = new LinkedHashMap<String, String>();
        for (Entry<JsonPointer, JsonNode> entry : bundle.entrySet()) {
            output.put(entry.getKey().toString(), entry.getValue().toString());
        }
        return output;
    }

    public void put(JsonPointer key, JsonNode value) {
        bundle.put(key, value);
    }

    public boolean isEmpty() {
        return bundle.isEmpty();
    }

    public JsonNode get(JsonPointer jsonPointer) {
        return bundle.get(jsonPointer);
    }

    public int size() {
        return bundle.size();
    }

    /**
     * Returns a new TagBundle that is a merge of this bundle and the other TagBundle in a way that:
     * <ul>
     *     <li>result TagBundle contains all elements from this TagBundle that are not present in other TagBundle</li>
     *     <li>result TagBundle contains all elements from other TagBundle that are not present in this TagBundle</li>
     *     <li>element from result TagBundle is removed if respective element in other TagBundle is a JsonNode of JsonNodeType.NULL</li>
     *     <li>JsonNodeType.OBJECT elements present in both TagBundles are merged into element from result TagBundle</li>
     * </ul>
     * <p>
     * JsonNodeType.OBJECT elements are merged by field names in a way that:
     * <ul>
     *     <li>left object fields gets modified</li>
     *     <li>fields from right object that are not present in left object are added to left object</li>
     *     <li>fields in left object that are present in right object are replaced in left object by right object value</li>
     *     <li>field from left object is removed if the field in right object is null</li>
     * </ul>
     *
     * @param other TagBundle containing values that needs to be merged into this TagBundle
     * @return new TagBundle merged from a copy of this TagBundle and passed TagBundle
     */
    public TagBundle copyMerge(TagBundle other) {
        val copy = new TagBundle(this);
        for (Entry<JsonPointer, JsonNode> entry : other.bundle.entrySet()) {
            val key = entry.getKey();
            val value = entry.getValue();
            val nodeType = value.getNodeType();
            if (Objects.equals(nodeType, NULL)) {
                copy.bundle.remove(key);
            } else if (Objects.equals(nodeType, OBJECT)) {
                JsonNode existingTag = copy.get(key);
                if (existingTag == null) {
                    existingTag = value;
                } else {
                    mergeRightIntoLeft((ObjectNode) existingTag, (ObjectNode) value);
                }
                copy.put(key, existingTag);
            } else {
                copy.put(key, value);
            }
        }
        return copy;
    }

    private void mergeRightIntoLeft(ObjectNode left, ObjectNode right) {
        val fieldNames = right.fieldNames();
        while (fieldNames.hasNext()) {
            val key = fieldNames.next();
            val jsonNode = right.get(key);
            if (jsonNode.isNull()) {
                left.remove(key);
            } else {
                left.replace(key, jsonNode);
            }
        }
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof TagBundle)) {
            return false;
        }
        TagBundle otherBundle = (TagBundle) o;
        return Objects.equals(bundle, otherBundle.bundle);
    }

    @Override
    public int hashCode() {
        return Objects.hash(bundle);
    }
}
