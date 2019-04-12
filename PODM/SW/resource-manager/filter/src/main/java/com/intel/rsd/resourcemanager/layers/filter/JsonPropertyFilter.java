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
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.google.common.base.Splitter;
import com.intel.rsd.java.lang.Numbers;
import lombok.NonNull;
import lombok.val;

import java.util.List;
import java.util.Objects;
import java.util.stream.IntStream;

import static com.intel.rsd.optional.OptionalUtils.stream;
import static java.util.Collections.singletonList;

final class JsonPropertyFilter {
    private final List<String> path;

    JsonPropertyFilter(@NonNull String filter) {
        path = Splitter.on('/').omitEmptyStrings().splitToList(filter);
    }

    void filter(@NonNull ObjectNode json) {
        filterObject(0, json);
    }

    private void filter(int index, JsonNode json) {
        if (json instanceof ArrayNode) {
            filterArray(index, (ArrayNode) json);
        } else if (json instanceof ObjectNode) {
            filterObject(index, (ObjectNode) json);
        }
    }

    private void filterObject(int index, ObjectNode object) {
        String currentSegment = path.get(index);

        val fieldNames = isWildcard(currentSegment)
            ? object.fieldNames()
            : singletonList(currentSegment).iterator();

        if (isLastSegment(index)) {
            fieldNames.forEachRemaining(object::remove);
        } else {
            fieldNames.forEachRemaining(f -> filter(index + 1, object.get(f)));
        }
    }

    private void filterArray(int index, ArrayNode array) {
        String currentSegment = path.get(index);

        val indexes = isWildcard(currentSegment)
            ? IntStream.range(0, array.size()).boxed()
            : stream(Numbers.tryParseInt(currentSegment));

        if (isLastSegment(index)) {
            indexes.forEach(array::remove);
        } else {
            indexes.forEach(i -> filter(index + 1, array.get(i)));
        }
    }

    private boolean isWildcard(String segment) {
        return Objects.equals(segment, "*");
    }

    private boolean isLastSegment(int index) {
        return index == path.size() - 1;
    }
}
