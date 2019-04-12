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

package com.intel.rsd.resourcemanager.layers.merger.resources;

import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.common.resources.ResourceProvider;
import lombok.val;

import java.io.IOException;
import java.util.StringJoiner;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;

public class CollectionResource implements ResourceProvider {

    private final ObjectNode collectionResource;
    private String odataId;

    public CollectionResource(String odataId, String collectionName) throws IOException {
        this.odataId = odataId;
        this.collectionResource = readJsonFromFile(this, "Collection.json");
        val odataType = createOdataTypeForCollection(collectionName);
        val odataContext = "/redfish/v1/$metadata" + odataType;
        collectionResource.put("@odata.id", odataId);
        collectionResource.put("@odata.type", odataType);
        collectionResource.put("@odata.context", odataContext);
        collectionResource.put("Name", splitCamelCase(collectionName));
    }

    @Override
    public String getODataId() {
        return odataId;
    }

    @Override
    public ObjectNode getResource() {
        return collectionResource.deepCopy();
    }

    private String createOdataTypeForCollection(String collectionName) {
        return new StringJoiner(".", "#", "").add(collectionName).add(collectionName).toString();
    }

    private static String splitCamelCase(String s) {
        return s.replaceAll(
            String.format("%s|%s|%s",
                "(?<=[A-Z])(?=[A-Z][a-z])",
                "(?<=[^A-Z])(?=[A-Z])",
                "(?<=[A-Za-z])(?=[^A-Za-z])"
            ),
            " "
        );
    }
}
