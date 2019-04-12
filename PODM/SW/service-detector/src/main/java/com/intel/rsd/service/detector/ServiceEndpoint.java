/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.service.detector;

import com.intel.rsd.service.detector.data.RemoteServiceType;
import lombok.NonNull;

import java.net.URI;
import java.util.Objects;
import java.util.UUID;

import static java.lang.String.format;

/**
 * Represents REST API entry point of external service
 */
public final class ServiceEndpoint {

    private final UUID serviceUuid;
    private final URI endpointUri;
    private final RemoteServiceType remoteServiceType;

    public ServiceEndpoint(@NonNull UUID serviceUuid, @NonNull URI endpointUri, @NonNull RemoteServiceType remoteServiceType) {
        this.serviceUuid = serviceUuid;
        this.endpointUri = endpointUri;
        this.remoteServiceType = remoteServiceType;
    }

    public URI getEndpointUri() {
        return endpointUri;
    }

    public UUID getServiceUuid() {
        return serviceUuid;
    }

    public RemoteServiceType getRemoteServiceType() {
        return remoteServiceType;
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) {
            return true;
        }
        if (!(o instanceof ServiceEndpoint)) {
            return false;
        }
        ServiceEndpoint endpoint = (ServiceEndpoint) o;
        return Objects.equals(getServiceUuid(), endpoint.getServiceUuid())
            && Objects.equals(getEndpointUri(), endpoint.getEndpointUri());
    }

    @Override
    public int hashCode() {
        return Objects.hash(getServiceUuid());
    }

    @Override
    public String toString() {
        return format("{URI=%s, type=%s, UUID=%s}", endpointUri, remoteServiceType, serviceUuid);
    }
}
