/*
 * Copyright (c) 2016-2019 Intel Corporation
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

import com.intel.rsd.nodecomposer.types.net.HttpStatusCode;

import java.net.URI;
import java.util.Objects;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.types.net.HttpStatusCode.httpStatusCode;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;

public final class HttpResponse {
    private final HttpStatusCode statusCode;
    private final Object entity;
    private final URI location;

    public HttpResponse(int statusCode, Object entity, URI location) {
        this.statusCode = httpStatusCode(statusCode);
        this.entity = entity;
        this.location = location;
    }

    public HttpStatusCode getStatusCode() {
        return statusCode;
    }

    public Optional<Object> getEntity() {
        return ofNullable(entity);
    }

    public Optional<URI> getLocation() {
        return ofNullable(location);
    }

    @Override
    public boolean equals(Object other) {
        if (other == null || !(other instanceof HttpResponse)) {
            return false;
        }

        HttpResponse that = (HttpResponse) other;

        return Objects.equals(statusCode, that.statusCode)
            && Objects.equals(entity, that.entity)
            && Objects.equals(location, that.location);
    }

    @Override
    public int hashCode() {
        return Objects.hash(statusCode, entity, location);
    }

    @Override
    public String toString() {
        return format("HTTP{status=%s, entity=%s, location=%s}", statusCode, entity, location);
    }
}
