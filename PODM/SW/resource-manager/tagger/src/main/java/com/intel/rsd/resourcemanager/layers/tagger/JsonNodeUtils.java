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
import lombok.experimental.UtilityClass;
import lombok.val;

import static com.intel.rsd.resourcemanager.layers.tagger.JsonPointerUtils.leafNameOf;
import static com.intel.rsd.resourcemanager.layers.tagger.JsonPointerUtils.parentOf;
import static org.apache.commons.lang3.StringUtils.isEmpty;

@UtilityClass
class JsonNodeUtils {
    /**
     * Removes the parent container node of JsonPointer pointer from JsonNode body if the container node is empty.
     * This step is repeated up to root of JsonNode body.
     * <p>
     * If forceRemoval parameter is set then this method removes and returns a JsonNode located at JsonPointer pointer.
     *
     * @param body JsonNode that is being modified by this method
     * @param pointer JsonPointer with "leaf" node - this node's parent path up to root will be cleared if empty
     * @param forceRemoval boolean indicating if the "leaf" node should be removed first.
     * @return JsonNode located at "leaf" node
     */
    static JsonNode removePropertyPathRecursively(JsonNode body, JsonPointer pointer, boolean forceRemoval) {
        if (isEmpty(pointer.toString())) {
            return null;
        }
        val jsonNode = body.at(pointer);
        if (!jsonNode.iterator().hasNext() || forceRemoval) {
            val parentJsonPointer = parentOf(pointer);
            val parentNode = (ObjectNode) body.at(parentJsonPointer);

            parentNode.remove(leafNameOf(pointer));
            removePropertyPathRecursively(body, parentJsonPointer, false);
        }
        return jsonNode;
    }

    /**
     * Places a JsonNode jsonNodeToBePut at JsonPointer pointer in JsonNode container. Modifies the passed JsonNode container.
     * As a result all non-existent objects in JsonPointer pointer path will be created, e.g.:
     *   JsonNode container: {}
     *   JsonPointer pointer: "/some/json/pointer"
     *   JsonNode jsonNodeToBePut: "textValue"
     *
     *   Updated JsonNode container: {
     *      "some": {
     *          "json": {
     *              "pointer": "textValue"
     *          }
     *      }
     *   }
     *
     * If a value already exists at JsonPointer pointer it will be replaced by JsonNode jsonNodeToBePut.
     *
     * @param container JsonNode that is being modified by this method
     * @param pointer location where to put the value
     * @param jsonNodeToBePut value to be placed
     */
    static void putNodeAtPointer(JsonNode container, JsonPointer pointer, JsonNode jsonNodeToBePut) {
        val parentNode = createParentPathRecursively(container, pointer);
        parentNode.replace(leafNameOf(pointer), jsonNodeToBePut);
    }

    private static ObjectNode createParentPathRecursively(JsonNode jsonNode, JsonPointer jsonPointer) {
        val parentJsonPointer = parentOf(jsonPointer);
        val jsonNodeAtPointer = jsonNode.at(parentJsonPointer);

        if (jsonNodeAtPointer.isMissingNode()) {
            val parent = createParentPathRecursively(jsonNode, parentJsonPointer);
            return parent.putObject(leafNameOf(parentJsonPointer));
        } else {
            return (ObjectNode) jsonNodeAtPointer;
        }
    }
}
