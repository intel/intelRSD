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

package com.intel.rsd.resourcemanager.layers;

import com.fasterxml.jackson.databind.JsonNode;
import lombok.NonNull;
import lombok.ToString;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;

@ToString
public class Response {
    private JsonNode body;

    private HttpHeaders httpHeaders;

    private HttpStatus httpStatus;

    public Response() {
    }

    public Response(@NonNull ResponseEntity<JsonNode> entity) {
        this(entity.getBody(), entity.getHeaders(), entity.getStatusCode());
    }

    public Response(JsonNode body, @NonNull HttpHeaders httpHeaders, @NonNull HttpStatus httpStatus) {
        this.body = body;
        this.httpHeaders = httpHeaders;
        this.httpStatus = httpStatus;
    }

    public Response(JsonNode body, @NonNull HttpStatus httpStatus) {
        this.body = body;
        this.httpHeaders = new HttpHeaders();
        this.httpStatus = httpStatus;
    }

    public JsonNode getBody() {
        return body;
    }

    public HttpHeaders getHttpHeaders() {
        return httpHeaders;
    }

    public HttpStatus getHttpStatus() {
        return httpStatus;
    }

    public ResponseEntity<JsonNode> toResponseEntity() {
        return new ResponseEntity<>(getBody(), getHttpHeaders(), getHttpStatus());
    }
}
