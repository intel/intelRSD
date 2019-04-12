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
import lombok.val;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.List;

import static com.intel.rsd.json.JsonUtils.stringToJsonNode;
import static com.intel.rsd.resourcemanager.layers.tagger.TagBundle.newTagBundle;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertNotSame;
import static org.testng.Assert.assertTrue;

public class TagBundleTest {
    @Test
    public void whenEmptyTagBundlesAreMerged_resultShouldBeANewEmptyTagBundle() {
        TagBundle left = newTagBundle();
        val merged = left.copyMerge(newTagBundle());
        assertTrue(merged.isEmpty());
        assertNotSame(left, merged);
    }

    @Test
    @SuppressWarnings({"checkstyle:ExecutableStatementCount", "checkstyle:MagicNumber"})
    public void whenTagBundlesContainNotNullAndDifferentElements_resultShouldContainAllThoseElements() {
        val right = newTagBundle();
        val pointer1 = JsonPointer.compile("/some/path");
        val element1 = stringToJsonNode("{}");
        right.put(pointer1, element1);
        val pointer2 = JsonPointer.compile("/some/other/path");
        val element2 = stringToJsonNode("{\"other\":\"value\"}");
        right.put(pointer2, element2);

        val left = newTagBundle();
        val pointer3 = JsonPointer.compile("/different/path");
        val element3 = stringToJsonNode("{\"different\":\"value\"}");
        left.put(pointer3, element3);

        val merged = left.copyMerge(right);

        assertEquals(merged.size(), 3);
        assertEquals(merged.get(pointer1), element1);
        assertEquals(merged.get(pointer2), element2);
        assertEquals(merged.get(pointer3), element3);
    }

    @Test
    public void whenRightTagBundleHasNullElements_resultShouldNotContainThoseElements() {
        val right = newTagBundle();
        val pointer1 = JsonPointer.compile("/some/path");
        val element1 = stringToJsonNode("null");
        right.put(pointer1, element1);
        val pointer2 = JsonPointer.compile("/some/other/path");
        val element2 = stringToJsonNode("null");
        right.put(pointer2, element2);

        val merged = newTagBundle().copyMerge(right);
        assertTrue(merged.isEmpty());
    }

    @Test
    public void whenRightTagBundleHasNullElementsMatchingKeysFromLeftTagBundle_resultShouldNotContainThoseElements() {
        val right = newTagBundle();
        val matchingKey1 = JsonPointer.compile("/some/path");
        val element1 = stringToJsonNode("null");
        right.put(matchingKey1, element1);

        val matchingKey2 = JsonPointer.compile("/some/other/path");
        val element2 = stringToJsonNode("null");
        right.put(matchingKey2, element2);

        val left = newTagBundle();

        val element3 = stringToJsonNode("{\"any\":\"value\"}");
        left.put(matchingKey1, element3);

        val element4 = stringToJsonNode("{\"other\":\"value\"}");
        left.put(matchingKey2, element4);

        val merged = left.copyMerge(right);
        assertTrue(merged.isEmpty());
    }

    @Test
    public void whenMergingElementPresentInBothTagBundles_shouldRemoveFieldsFromLeftTagBundleThatAreNullInRightTagBundle() {
        val matchingKey = JsonPointer.compile("/some/path");
        val left = newTagBundle();
        val leftObject = stringToJsonNode("{\"matchingField\": \"someValue\"}");
        left.put(matchingKey, leftObject);

        val right = newTagBundle();
        val rightObject = stringToJsonNode("{\"matchingField\": null}");
        right.put(matchingKey, rightObject);

        val merged = left.copyMerge(right);

        JsonNode resultingNode = merged.get(matchingKey);
        assertTrue(!resultingNode.iterator().hasNext());
    }

    @Test
    public void whenMergingElementPresentInBothTagBundlesThatContainDifferentFields_shouldContainAllFieldsFromBothElements() {
        val matchingKey = JsonPointer.compile("/some/path");
        val left = newTagBundle();
        val leftObject = stringToJsonNode("{\"someField\": \"someValue\"}");
        left.put(matchingKey, leftObject);

        val right = newTagBundle();
        val rightObject = stringToJsonNode("{\"anyOtherField\": \"anyValue\"}");
        right.put(matchingKey, rightObject);

        val merged = left.copyMerge(right);
        JsonNode resultingNode = merged.get(matchingKey);
        assertEquals(resultingNode.size(), 2);

        List<String> fieldNames = new ArrayList<>();
        resultingNode.fieldNames().forEachRemaining(fieldNames::add);

        assertTrue(fieldNames.contains("someField"));
        assertTrue(fieldNames.contains("anyOtherField"));
    }

    @Test
    public void whenMergingElementPresentInBothTagBundlesThatContainSameField_shouldOverwriteLeftFieldValueWithRightFieldValue() {
        val matchingKey = JsonPointer.compile("/some/path");
        val left = newTagBundle();
        val leftObject = stringToJsonNode("{\"someField\": \"someValue\"}");
        left.put(matchingKey, leftObject);

        val right = newTagBundle();
        val rightObject = stringToJsonNode("{\"someField\": \"anyValue\"}");
        right.put(matchingKey, rightObject);

        val merged = left.copyMerge(right);
        JsonNode resultingNode = merged.get(matchingKey);
        assertEquals(resultingNode.size(), 1);
        assertEquals(resultingNode.get("someField").textValue(), "anyValue");
    }
}
