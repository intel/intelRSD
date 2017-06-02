/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.client.redfish.response;

import com.intel.podm.client.api.redfish.response.RedfishEntityResponseBody;
import com.intel.podm.client.api.redfish.response.RedfishResponse;
import com.intel.podm.client.redfish.http.HttpResponse;

import javax.ws.rs.core.Response.Status;
import java.net.URI;
import java.util.Optional;

import static com.intel.podm.common.utils.Contracts.requires;
import static com.intel.podm.common.utils.Contracts.requiresNonNull;

public final class RedfishResponseImpl implements RedfishResponse {
    private final RedfishEntityResponseBodyImpl entity;
    private final URI location;
    private final Status status;

    RedfishResponseImpl(HttpResponse response) {
        requiresNonNull(response, "response");
        response.getEntity().ifPresent(e -> {
            requires(e instanceof RedfishEntityResponseBodyImpl, "response.entity must be of RedfishEntityResponseBodyImpl type");
        });

        this.entity = response.getEntity().map(RedfishEntityResponseBodyImpl.class::cast).orElse(null);
        this.location = response.getLocation().orElse(null);
        this.status = Status.fromStatusCode(response.getStatusCode().toInt());
    }

    @Override
    public Optional<RedfishEntityResponseBody> getEntity() {
        return Optional.ofNullable(entity);
    }

    @Override
    public RedfishEntityResponseBody getEntityOrNull() {
        return entity;
    }

    @Override
    public Optional<URI> getLocation() {
        return Optional.ofNullable(location);
    }

    @Override
    public Status getStatus() {
        return status;
    }

    @Override
    public String toString() {
        return String.format("Response{body=%s, location=%s}", entity, location);
    }
}
