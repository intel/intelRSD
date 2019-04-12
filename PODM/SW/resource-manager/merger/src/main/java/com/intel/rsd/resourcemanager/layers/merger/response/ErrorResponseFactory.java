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

package com.intel.rsd.resourcemanager.layers.merger.response;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ArrayNode;
import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import com.intel.rsd.resourcemanager.layers.Response;
import lombok.experimental.UtilityClass;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;

import java.util.Arrays;

import static org.springframework.http.HttpStatus.NOT_FOUND;

@UtilityClass
final class ErrorResponseFactory {
    private static final JsonNodeFactory FACTORY = JsonNodeFactory.instance;

    public static Response createNotFoundResponse() {
        return createErrorResponse("Base.1.0.InvalidEndpoint", "Invalid endpoint", NOT_FOUND);
    }

    public static Response createErrorResponse(String code, String message, HttpStatus status, String... extendedInfoMessages) {
        return new Response(createError(code, message, status, extendedInfoMessages));
    }

    static ResponseEntity<JsonNode> createError(String code, String message, HttpStatus status, String[] extendedInfoMessages) {
        ObjectNode errorBody = FACTORY.objectNode();
        ArrayNode extendedInfos = FACTORY.arrayNode();
        Arrays.stream(extendedInfoMessages)
            .map(s -> FACTORY.objectNode().put("Message", s).put("MessageId", "Base.1.0.ExternalServiceError"))
            .forEach(extendedInfos::add);
        JsonNode errorCode = FACTORY.objectNode()
            .put("code", code)
            .put("message", message)
            .set("@Message.ExtendedInfo", extendedInfos);

        errorBody.set("error", errorCode);
        return ResponseEntity.status(status).body(errorBody);
    }
}
