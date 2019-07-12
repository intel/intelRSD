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

package com.intel.rsd.resourcemanager.common;

import com.fasterxml.jackson.core.JsonFactory;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.ObjectMapper;
import lombok.experimental.UtilityClass;
import lombok.val;
import org.springframework.http.HttpHeaders;

import javax.servlet.http.HttpServletRequest;
import java.io.IOException;

@UtilityClass
public class HttpServletRequestUtils {

    private static final ObjectMapper MAPPER = new ObjectMapper();

    private static final JsonFactory JSON_FACTORY = new JsonFactory();

    public static JsonNode readBody(HttpServletRequest httpServletRequest) throws IOException {
        try (JsonParser parser = JSON_FACTORY.createParser(httpServletRequest.getReader())) {
            return parser.nextToken() == null
                ? null
                : MAPPER.readValue(parser, JsonNode.class);
        }
    }

    public static HttpHeaders getHttpHeaders(HttpServletRequest httpServletRequest) {
        val httpHeaders = new HttpHeaders();
        val headerNames = httpServletRequest.getHeaderNames();
        while (headerNames.hasMoreElements()) {
            val headerName = headerNames.nextElement();
            val headerValue = httpServletRequest.getHeader(headerName);
            httpHeaders.add(headerName, headerValue);
        }
        return httpHeaders;
    }
}
