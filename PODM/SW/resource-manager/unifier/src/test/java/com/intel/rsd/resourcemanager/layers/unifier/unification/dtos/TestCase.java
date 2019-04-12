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

package com.intel.rsd.resourcemanager.layers.unifier.unification.dtos;

import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.crawler.Resource;
import lombok.Getter;

import java.util.List;
import java.util.stream.Collectors;

public final class TestCase {
    @Getter
    private List<UnificationMapping> assertions;
    private List<ObjectNode> resources;

    public List<Resource> getResources() {
        return resources
            .stream()
            .map(node -> new Resource() {
                @Override
                public JsonNode get(String jsonPointer) {
                    return node.at(JsonPointer.compile(jsonPointer));
                }

                @Override
                public List<JsonNode> getByPath(String jsonPath) {
                    return null;
                }
            }).collect(Collectors.toList());
    }
}
