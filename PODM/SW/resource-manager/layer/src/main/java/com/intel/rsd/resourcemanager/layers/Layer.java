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
import lombok.val;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

public abstract class Layer implements Chainable<Layer> {

    protected Layer nextLayer;

    @Override
    public final void setNext(Layer next) {
        this.nextLayer = next;
    }

    public Layer getNextLayer() {
        return nextLayer;
    }

    public abstract Set<ServiceId> getServices();

    public Collection<ServiceId> getServicesExposingPath(String path) {
        return getNextLayer().getServicesExposingPath(path);
    }

    public Response invoke(@NonNull ServiceId serviceId, @NonNull String path, @NonNull HttpMethod method, @NonNull HttpHeaders headers, JsonNode body,
                           Map<String, String> requestParams) {
        return invokeImpl(serviceId, path, method, headers, body, requestParams);
    }

    public Response invoke(@NonNull ServiceId serviceId, @NonNull String path, @NonNull HttpMethod method, @NonNull HttpHeaders headers, JsonNode body) {
        return invokeImpl(serviceId, path, method, headers, body, null);
    }

    public final Response invoke(@NonNull ServiceId serviceId, @NonNull String path, @NonNull HttpMethod method) {
        return invoke(serviceId, path, method, new HttpHeaders(), null);
    }

    public final Response invoke(@NonNull ServiceId serviceId, @NonNull String path, @NonNull HttpMethod method, @NonNull HttpHeaders headers) {
        return invoke(serviceId, path, method, headers, null);
    }

    protected abstract Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                           Map<String, String> requestParams);

    public Response passToNextLayer(
        @NonNull ServiceId serviceId,
        @NonNull String path,
        @NonNull HttpMethod method,
        @NonNull HttpHeaders headers,
        JsonNode body,
        Map<String, String> requestParams) {

        val httpHeadersCopy = new HttpHeaders();
        httpHeadersCopy.putAll(headers);

        return getNextLayer().invoke(serviceId, path, method, httpHeadersCopy, body, requestParams);
    }
}
