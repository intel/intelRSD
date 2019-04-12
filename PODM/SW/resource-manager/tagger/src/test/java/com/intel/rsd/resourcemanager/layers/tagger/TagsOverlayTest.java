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

package com.intel.rsd.resourcemanager.layers.tagger;

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.JsonNodeType;
import lombok.val;
import org.testng.annotations.Test;

import static com.fasterxml.jackson.databind.node.JsonNodeType.ARRAY;
import static com.fasterxml.jackson.databind.node.JsonNodeType.OBJECT;
import static com.fasterxml.jackson.databind.node.JsonNodeType.STRING;
import static com.intel.rsd.json.JsonUtils.newObjectNode;
import static com.intel.rsd.resourcemanager.layers.tagger.TagsOverlay.putDefaultTagInJsonNode;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertTrue;

public class TagsOverlayTest {
    private static final JsonPointer JSONPOINTER = JsonPointer.compile("/Oem/Intel_RackScale/TaggedValues");

    @Test
    public void whenSettingCorrectJsonObject_ShouldWriteItToObjectMarkedProperty() {
        val expectedJson = newObjectNode().put("abc", "bcd");
        val tagDefinition = prepareTagDefinition(JSONPOINTER, OBJECT, expectedJson.toString());
        val jsonNode = newObjectNode();
        putDefaultTagInJsonNode(jsonNode, tagDefinition, JSONPOINTER);
        assertEquals(jsonNode.at(JSONPOINTER), expectedJson);
    }

    @Test
    public void whenSettingCorrectJsonArray_ShouldWriteItToArrayMarkedProperty() {
        val jsonNode = newObjectNode();
        val firstObj = newObjectNode().put("a", "b");
        val secondObj = newObjectNode().put("1", "2");
        val expectedJsonNode = newObjectNode().arrayNode().add(firstObj).add(secondObj);
        val tagDefinition = prepareTagDefinition(JSONPOINTER, ARRAY, expectedJsonNode.toString());
        putDefaultTagInJsonNode(jsonNode, tagDefinition, JSONPOINTER);
        assertEquals(jsonNode.at(JSONPOINTER), expectedJsonNode);
    }

    @Test
    public void whenWritingStringToStringProperty_ShouldReceiveStringAsResult() {
        val jsonNode = newObjectNode();
        val defaultValue = "someString{'a':'b'}";
        val tagDefinition = prepareTagDefinition(JSONPOINTER, STRING, defaultValue);
        putDefaultTagInJsonNode(jsonNode, tagDefinition, JSONPOINTER);
        assertEquals(jsonNode.at(JSONPOINTER).asText(), defaultValue);
    }

    @Test
    public void whenTryingToWriteStringIntoObjectMarkedProperty_ShouldReceiveNullNode() {
        val jsonNode = newObjectNode();
        val defaultValue = "stringThatIsntObject";
        val tagDefinition = prepareTagDefinition(JSONPOINTER, OBJECT, defaultValue);
        putDefaultTagInJsonNode(jsonNode, tagDefinition, JSONPOINTER);
        assertTrue(jsonNode.at(JSONPOINTER).isNull());
    }

    @Test
    public void whenWritingEmptyStringIntoAnyProperty_ShouldReceiveNullProperty() {
        val jsonNode = newObjectNode();
        val defaultValue = "";
        val tagDefinition = prepareTagDefinition(JSONPOINTER, ARRAY, defaultValue);
        putDefaultTagInJsonNode(jsonNode, tagDefinition, JSONPOINTER);
        assertEquals(jsonNode.at(JSONPOINTER), JsonNodeFactory.instance.nullNode());
    }

    private TagDefinition prepareTagDefinition(JsonPointer jsonPointer, JsonNodeType jsonNodeType, String defaultValue) {
        val tagDefinition = new TagDefinition();
        tagDefinition.setResource("/redfish/v1/**");
        tagDefinition.setProperty(jsonPointer);
        tagDefinition.setType(jsonNodeType);
        tagDefinition.setDefaultValue(defaultValue);
        return tagDefinition;
    }
}
