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

package com.intel.rsd.json;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.experimental.UtilityClass;

@UtilityClass
public class JsonNodeUtils {
    public static Integer integerOrNull(JsonNode jsonNode, String jsonPath) {
        JsonNode possibleIntegerNode = jsonNode.at(jsonPath);
        return possibleIntegerNode.canConvertToInt() ? possibleIntegerNode.asInt() : null;
    }

    public static Long longOrNull(JsonNode jsonNode, String jsonPath) {
        JsonNode possibleLongNode = jsonNode.at(jsonPath);
        return longOrNull(possibleLongNode);
    }

    public static Long longOrNull(JsonNode possibleLongNode) {
        return possibleLongNode.canConvertToLong() ? possibleLongNode.asLong() : null;
    }
}
