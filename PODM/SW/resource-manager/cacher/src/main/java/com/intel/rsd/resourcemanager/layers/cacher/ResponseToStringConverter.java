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

package com.intel.rsd.resourcemanager.layers.cacher;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.SneakyThrows;

public class ResponseToStringConverter {

    public static final TypeReference<Response> RESPONSE_TYPE_REFERENCE = new TypeReference<Response>() {
    };

    private ObjectMapper objectMapper = new ObjectMapper();

    @SneakyThrows
    public String convertToDatabaseColumn(Response attribute) {
        return objectMapper.writeValueAsString(attribute);
    }

    @SneakyThrows
    public Response convertToEntityAttribute(String dbData) {
        return objectMapper.readValue(dbData, RESPONSE_TYPE_REFERENCE);
    }
}
