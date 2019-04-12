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

package com.intel.rsd.nodecomposer.externalservices.redfish.http;

import com.intel.rsd.nodecomposer.types.net.HttpMethod;

import java.net.URI;
import java.util.Objects;
import java.util.Optional;

import static java.lang.String.format;
import static java.util.Optional.ofNullable;

public final class HttpRequest {
    private final HttpMethod httpMethod;
    private final URI requestUri;
    private final Object requestBody;

    public HttpRequest(HttpMethod httpMethod, URI requestUri, Object requestBody) {
        this.httpMethod = httpMethod;
        this.requestUri = requestUri;
        this.requestBody = requestBody;
    }

    public HttpMethod getHttpMethod() {
        return httpMethod;
    }

    public URI getRequestUri() {
        return requestUri;
    }

    public Optional<Object> getRequestBody() {
        return ofNullable(requestBody);
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof HttpRequest)) {
            return false;
        }
        HttpRequest that = (HttpRequest) o;
        return getHttpMethod() == that.getHttpMethod()
            && Objects.equals(getRequestUri(), that.getRequestUri())
            && Objects.equals(getRequestBody(), that.getRequestBody());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getHttpMethod(), getRequestUri(), getRequestBody());
    }

    @Override
    public String toString() {
        return format("HTTP{method=%s, location=%s, body=%s}", httpMethod, requestUri, requestBody);
    }
}
