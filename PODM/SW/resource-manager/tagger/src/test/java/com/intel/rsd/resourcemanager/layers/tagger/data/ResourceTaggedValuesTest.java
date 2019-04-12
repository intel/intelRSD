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

package com.intel.rsd.resourcemanager.layers.tagger.data;

import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.tagger.TagBundle;
import lombok.val;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.stream.Stream;

import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertTrue;

public class ResourceTaggedValuesTest {
    @DataProvider
    public static Object[][] emptySerializedTaggedValues() {
        return Stream.of("null", "{}").map(value -> new Object[]{value}).toArray(Object[][]::new);
    }

    @Test(dataProvider = "emptySerializedTaggedValues")
    public void whenGettingTaggedValuesForNullOrEmptySerializedValues_shouldReturnEmptyTagBundle(String serializedTaggedValue) {
        val values = new ResourceTaggedValues(new ResourceReference(new ServiceId("service"), "path"), serializedTaggedValue);
        TagBundle tagBundle = values.getTaggedValues();
        assertNotNull(tagBundle);
        assertTrue(tagBundle.isEmpty());
    }
}
