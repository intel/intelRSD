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

import com.fasterxml.jackson.core.type.TypeReference;
import lombok.SneakyThrows;
import lombok.experimental.UtilityClass;

import java.util.LinkedHashMap;

import static com.intel.rsd.json.JsonUtils.readUsingTypeRef;

@UtilityClass
public class TaggedValuesSerializerDeserializer {
    private static final TypeReference<LinkedHashMap<String, String>> TYPE_REF = new TypeReference<LinkedHashMap<String, String>>() {
    };

    @SneakyThrows
    static LinkedHashMap<String, String> asMap(String serializedMap) {
        LinkedHashMap<String, String> storedValues = readUsingTypeRef(serializedMap, TYPE_REF);
        if (storedValues == null) {
            return new LinkedHashMap<>();
        } else {
            return storedValues;
        }
    }
}
