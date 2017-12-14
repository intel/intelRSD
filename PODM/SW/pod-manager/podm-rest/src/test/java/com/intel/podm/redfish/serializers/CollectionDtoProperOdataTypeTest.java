/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.redfish.serializers;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.Iterator;
import java.util.Objects;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import static com.intel.podm.redfish.serializers.CollectionTypeToCollectionODataMapping.COLLECTION_TYPE_TO_ODATA_MAPPING;
import static java.util.stream.Collectors.toList;
import static org.testng.Assert.fail;
import static org.testng.AssertJUnit.assertTrue;

public class CollectionDtoProperOdataTypeTest {
    private final Pattern regex = Pattern.compile("^#(?<namespace>[a-zA-Z0-9]+)\\.(?<name>[a-zA-Z0-9]+)$");

    @Test(dataProvider = "CollectionDtoOdataTypes")
    private void shouldPass(String odataType) {
        Matcher matcher = regex.matcher(odataType);
        if (matcher.matches()) {
            String namespace = matcher.group("namespace");
            String name = matcher.group("name");
            assertTrue(Objects.equals(name, namespace));
        } else {
            fail();
        }
    }

    @DataProvider(name = "CollectionDtoOdataTypes")
    public Iterator<Object[]> odataTypeInCollectionDtoShouldHaveProperFormat() {
        return COLLECTION_TYPE_TO_ODATA_MAPPING.values().stream()
            .map(oDataForCollection -> new Object[]{oDataForCollection.getODataType()})
            .collect(toList())
            .iterator();
    }

    @Test(dataProvider = "InvalidCollectionDtoOdataTypes")
    public void shouldFail(String odataType) {
        Matcher matcher = regex.matcher(odataType);
        boolean matches = matcher.matches();
        boolean partsAreEqual = false;
        if (matches) {
            String namespace = matcher.group("namespace");
            String name = matcher.group("name");
            partsAreEqual = Objects.equals(name, namespace);
        }
        assertTrue(!matches || !partsAreEqual);
    }

    @DataProvider(name = "InvalidCollectionDtoOdataTypes")
    public Object[][] invalidOdataTypes() {
        return new Object[][]{
            {"#Collection.v1.0.0.Collection"},
            {"Collection.Collection"},
            {"Collection.OtherCollection"},
            {"#Collection.OtherCollection"}
        };
    }
}
