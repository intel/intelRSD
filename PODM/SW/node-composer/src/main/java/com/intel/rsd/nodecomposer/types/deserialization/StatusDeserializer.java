/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.deserialization;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.intel.rsd.nodecomposer.types.Status;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

import static com.intel.rsd.nodecomposer.types.Status.statusFromMap;

public class StatusDeserializer extends JsonDeserializer<Status> {
    @Override
    public Status deserialize(JsonParser jsonParser, DeserializationContext context) throws IOException {
        Map<String, String> map = jsonParser.getCodec().
            readValue(jsonParser, new TypeReference<LinkedHashMap<String, Object>>() {
            });

        try {
            return statusFromMap(map);
        } catch (IllegalArgumentException e) {
            throw new JsonParseException("Unable to create Status: ", jsonParser.getCurrentLocation(), e);
        }
    }
}
