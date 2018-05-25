/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.rest;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.node.TextNode;
import com.intel.podm.common.logger.Logger;

import java.io.IOException;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import static com.fasterxml.jackson.databind.SerializationFeature.INDENT_OUTPUT;
import static com.intel.podm.common.logger.LoggerFactory.getLogger;

public class SensitiveParametersRequestFilter {
    private static final Logger LOGGER = getLogger(SensitiveParametersRequestFilter.class);

    private static final List<String> SECURE_PARAMETERS
        = Arrays.asList("CHAPSecret", "MutualCHAPSecret", "Secret", "MutualSecret", "Password");

    public String filterSecretPropertiesFromRequest(String request) {
        if (!hasSecretParameter(request)) {
            return request;
        }

        ObjectMapper mapper = new ObjectMapper().enable(INDENT_OUTPUT);
        JsonNode json = getJsonFromRequest(request, mapper);

        for (String secureParameter : SECURE_PARAMETERS) {
            removeValueFromParameter(json, secureParameter);
        }

        return createJsonString(mapper, json);
    }

    private JsonNode getJsonFromRequest(String request, ObjectMapper mapper) {
        JsonNode json = mapper.getNodeFactory().objectNode();
        try {
            json = mapper.readTree(request);
        } catch (IOException e) {
            LOGGER.w("Request is not in valid json format");
        }
        return json;
    }

    private String createJsonString(ObjectMapper mapper, JsonNode json) {
        String jsonString = "";
        try {
            jsonString = mapper.writeValueAsString(json);
        } catch (JsonProcessingException e) {
            LOGGER.e("error while creating string from json request");
        }
        return jsonString;
    }

    private boolean hasSecretParameter(String request) {
        return SECURE_PARAMETERS.stream().anyMatch(request::contains);
    }

    private void removeValueFromParameter(JsonNode json, String secureParameter) {
        Iterator<Map.Entry<String, JsonNode>> nodes = json.fields();
        TextNode emptyTextNode = new TextNode("");

        while (nodes.hasNext()) {
            Map.Entry<String, JsonNode> parameterNameAndNode = nodes.next();

            JsonNode node = parameterNameAndNode.getValue();
            if (node.isObject()) {
                removeValueFromParameter(node, secureParameter);
            }

            if (node.isArray()) {
                for (JsonNode arrayElement : node) {
                    removeValueFromParameter(arrayElement, secureParameter);
                }
            }

            if (parameterNameAndNode.getKey().equals(secureParameter)) {
                parameterNameAndNode.setValue(emptyTextNode);
            }
        }
    }
}
