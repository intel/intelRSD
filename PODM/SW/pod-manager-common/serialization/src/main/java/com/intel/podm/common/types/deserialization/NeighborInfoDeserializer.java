/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.common.types.deserialization;

import com.fasterxml.jackson.core.JsonParseException;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.DeserializationContext;
import com.fasterxml.jackson.databind.JsonDeserializer;
import com.intel.podm.common.types.NeighborInfo;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

public class NeighborInfoDeserializer extends JsonDeserializer<NeighborInfo> {
    @Override
    public NeighborInfo deserialize(JsonParser jsonParser, DeserializationContext context) throws IOException {
        Map<String, String> attributesMap = jsonParser.getCodec().readValue(jsonParser, new TypeReference<LinkedHashMap<String, String>>() {
        });

        try {
            return new NeighborInfo(attributesMap);
        } catch (IllegalArgumentException e) {
            throw new JsonParseException("Unable to create NeighborInfo: wrong attributes", jsonParser.getCurrentLocation(), e);
        }
    }
}
