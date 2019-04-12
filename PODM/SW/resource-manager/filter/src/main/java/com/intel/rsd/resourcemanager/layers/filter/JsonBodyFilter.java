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

package com.intel.rsd.resourcemanager.layers.filter;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.NonNull;
import lombok.val;

import java.util.Collection;
import java.util.List;

import static java.util.stream.Collectors.toList;

final class JsonBodyFilter {
    Collection<FilterPattern> filters;

    JsonBodyFilter(@NonNull List<String> filters) {
        this.filters = filters
            .stream()
            .map(FilterPatternFactory::createFilterPattern)
            .collect(toList());
    }

    public JsonNode filterBody(@NonNull String uri, JsonNode json) {
        if (json == null) {
            return null;
        }

        val result = (ObjectNode) json;
        filters.forEach(f -> f.process(uri, result));
        return result;
    }
}
