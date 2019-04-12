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

package com.intel.rsd.podm;

import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.redfish.RedfishError;
import lombok.val;
import org.springframework.cloud.netflix.zuul.filters.route.FallbackProvider;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.http.client.ClientHttpResponse;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.net.SocketTimeoutException;

import static com.intel.rsd.json.JsonUtils.jsonNodeToByteArray;
import static com.intel.rsd.json.JsonUtils.objectToJsonNode;
import static com.intel.rsd.redfish.RedfishErrors.createRedfishError;
import static org.springframework.http.HttpHeaders.CONTENT_TYPE;
import static org.springframework.http.HttpStatus.GATEWAY_TIMEOUT;
import static org.springframework.http.MediaType.APPLICATION_JSON_VALUE;

public class GatewayFallbackProvider implements FallbackProvider {
    @Override
    public String getRoute() {
        return "*";
    }

    @Override
    public ClientHttpResponse fallbackResponse(String route, Throwable cause) {
        if (cause instanceof SocketTimeoutException) {
            val redfishError = createRedfishError(GATEWAY_TIMEOUT, route + " does not respond in expected time.");
            return createClientHttpResponse(GATEWAY_TIMEOUT, redfishError);
        }
        throw new UnsupportedOperationException("No fallback available.");
    }

    private ClientHttpResponse createClientHttpResponse(HttpStatus httpStatus, RedfishError redfishError) {
        val responseEntity = ResponseEntity.status(httpStatus).header(CONTENT_TYPE, APPLICATION_JSON_VALUE).body(objectToJsonNode(redfishError));
        return new ResponseEntityWrapper(responseEntity);
    }

    static class ResponseEntityWrapper implements ClientHttpResponse {

        private final ResponseEntity<JsonNode> responseEntity;

        @SuppressWarnings({"unchecked"})
        ResponseEntityWrapper(ResponseEntity responseEntity) {
            this.responseEntity = responseEntity;
        }

        @Override
        public HttpStatus getStatusCode() {
            return responseEntity.getStatusCode();
        }

        @Override
        public int getRawStatusCode() {
            return responseEntity.getStatusCodeValue();
        }

        @Override
        public String getStatusText() {
            return null;
        }

        @Override
        public void close() {

        }

        @Override
        public InputStream getBody() {
            try {
                return new ByteArrayInputStream(jsonNodeToByteArray(responseEntity.getBody()));
            } catch (JsonProcessingException e) {
                return null;
            }
        }

        @Override
        public HttpHeaders getHeaders() {
            return responseEntity.getHeaders();
        }
    }
}
