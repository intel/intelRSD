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
import lombok.NonNull;

import java.io.IOException;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;

public class Resource implements ResourceProvider {

    private final ObjectNode serviceResource;
    private final String odataId;

    public Resource(@NonNull String jsonFileName) throws IOException {
        serviceResource = readJsonFromFile(this, jsonFileName);
        odataId = serviceResource.get("@odata.id").asText();
    }

    @Override
    public String getODataId() {
        return odataId;
    }

    @Override
    public ObjectNode getResource() {
        return serviceResource.deepCopy();
    }
}
