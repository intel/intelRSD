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

package com.intel.rsd.json;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.NonNull;
import lombok.SneakyThrows;

import java.io.IOException;
import java.io.InputStream;

public final class JsonUtils {
    private static final ObjectMapper MAPPER = new ObjectMapper();

    private JsonUtils() {
    }

    public static ObjectNode readJsonFromFile(@NonNull Object context, @NonNull String fileName) throws IOException {
        return readJsonFromFile(context.getClass(), fileName);
    }

    public static ObjectNode readJsonFromFile(@NonNull Class clazz, @NonNull String fileName) throws IOException {
        try (InputStream stream = clazz.getResourceAsStream(fileName)) {
            return MAPPER.readValue(stream, ObjectNode.class);
        }
    }

    public static <T> T readJsonFromFile(@NonNull Object context, @NonNull String fileName, @NonNull Class<T> target) throws IOException {
        try (InputStream stream = context.getClass().getResourceAsStream(fileName)) {
            return MAPPER.readValue(stream, target);
        }
    }

    public static ArrayNode createArrayNodeWith(@NonNull Object objectToAdd) {
        return JsonNodeFactory.instance.arrayNode()
            .add(MAPPER.valueToTree(objectToAdd));
    }

    public static ObjectNode newObjectNode() {
        return MAPPER.createObjectNode();
    }

    public static ArrayNode newArrayNode() {
        return MAPPER.createArrayNode();
    }

    public static JsonNode objectToJsonNode(@NonNull Object object) {
        return MAPPER.valueToTree(object);
    }

    @SneakyThrows
    public static JsonNode stringToJsonNode(@NonNull String string) {
        return MAPPER.readTree(string);
    }

    public static byte[] jsonNodeToByteArray(@NonNull JsonNode jsonNode) throws JsonProcessingException {
        return MAPPER.writeValueAsBytes(jsonNode);
    }

    public static String createStringWith(@NonNull Object object) throws JsonProcessingException {
        return MAPPER.writeValueAsString(object);
    }

    @SneakyThrows
    public static String jsonNodeAsString(@NonNull Object object) {
        return MAPPER.writeValueAsString(object);
    }

    public static <T> T readUsingTypeRef(String string, @NonNull TypeReference<T> typeReference) throws IOException {
        return MAPPER.readValue(string == null ? String.valueOf((Object) null) : string, typeReference);
    }

    public static ObjectNode readJsonFromStream(@NonNull InputStream stream) throws IOException {
        return MAPPER.readValue(stream, ObjectNode.class);
    }
}
