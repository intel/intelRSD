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

package com.intel.rsd.nodecomposer.externalservices.redfish.response;

import com.intel.rsd.nodecomposer.externalservices.redfish.http.HttpResponse;

import javax.ws.rs.core.Response.Status;
import java.net.URI;
import java.util.Optional;

import static com.intel.rsd.nodecomposer.utils.Contracts.requires;
import static com.intel.rsd.nodecomposer.utils.Contracts.requiresNonNull;
import static java.lang.String.format;
import static java.util.Optional.ofNullable;
import static javax.ws.rs.core.Response.Status.fromStatusCode;

public final class RedfishResponseImpl implements RedfishResponse {
    private final RedfishEntityResponseBodyImpl entity;
    private final URI location;
    private final Status status;

    RedfishResponseImpl(HttpResponse response) {
        requiresNonNull(response, "response");

        Optional<Object> responseEntity = response.getEntity();
        responseEntity.ifPresent(
            e -> requires(e instanceof RedfishEntityResponseBodyImpl, "response.entity must be of RedfishEntityResponseBodyImpl type")
        );

        this.entity = responseEntity.map(RedfishEntityResponseBodyImpl.class::cast).orElse(null);
        this.location = response.getLocation().orElse(null);
        this.status = fromStatusCode(response.getStatusCode().toInt());
    }

    @Override
    public Optional<RedfishEntityResponseBody> getEntity() {
        return ofNullable(entity);
    }

    @Override
    public RedfishEntityResponseBody getEntityOrNull() {
        return entity;
    }

    @Override
    public Optional<URI> getLocation() {
        return ofNullable(location);
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public String toString() {
        return format("Response{body=%s, location=%s}", entity, location);
    }
}
