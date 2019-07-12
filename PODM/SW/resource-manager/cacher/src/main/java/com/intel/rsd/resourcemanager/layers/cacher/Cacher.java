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

package com.intel.rsd.resourcemanager.layers.cacher;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.time.LocalDateTime;
import java.util.Map;
import java.util.Optional;
import java.util.Set;

import static com.intel.rsd.resourcemanager.layers.cacher.CacheHint.ALWAYS_CACHED;
import static com.intel.rsd.resourcemanager.layers.cacher.HttpHeadersExtensions.getMaxStale;
import static java.time.temporal.ChronoUnit.SECONDS;
import static java.util.Collections.singletonList;

@Component
@Slf4j
final class Cacher extends Layer {

    private final Cache cache;

    @Autowired
    Cacher(Cache cache) {
        this.cache = cache;
    }

    @Override
    public Set<ServiceId> getServices() {
        return getNextLayer().getServices();
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {

        switch (method) {
            case GET:
                return invokeReadOnlyRequest(serviceId, path, method, headers, body, requestParams);

            case POST:
            case PUT:
            case PATCH:
            case DELETE:
                return invokeReadWriteRequest(serviceId, path, method, headers, body, requestParams);

            default:
                return passToNextLayer(serviceId, path, method, headers, body, requestParams);

        }
    }

    private Response invokeReadWriteRequest(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                            Map<String, String> requestParams) {

        val response = passToNextLayer(serviceId, path, method, headers, body, requestParams);
        if (response.getHttpStatus().is2xxSuccessful()) {
            cache.remove(asResourceReference(serviceId, path));
        }
        return response;
    }

    private Response invokeReadOnlyRequest(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                           Map<String, String> requestParams) {

        val resourceReference = asResourceReference(serviceId, path);
        val maxStaleAsCacheHint = getMaxStale(headers).map(CacheHint::asCacheHint);
        return maxStaleAsCacheHint
            .flatMap(cacheHint -> tryFindCachedResponse(resourceReference, cacheHint))
            .orElseGet(() -> {
                val nextLayerResponse = passToNextLayer(serviceId, path, method, headers, body, requestParams);
                return processReadOnlyRequestResponse(nextLayerResponse, resourceReference, maxStaleAsCacheHint.orElse(ALWAYS_CACHED));
            });
    }

    @SuppressWarnings("checkstyle:CyclomaticComplexity")
    private Response processReadOnlyRequestResponse(Response nextLayerResponse, ResourceReference resourceReference, CacheHint cacheHint) {
        switch (nextLayerResponse.getHttpStatus()) {
            case OK:
                cache.updateOrCreate(resourceReference, nextLayerResponse);
                return nextLayerResponse;

            case NOT_FOUND:
                cache.remove(resourceReference);
                return nextLayerResponse;

            case INTERNAL_SERVER_ERROR:
            case BAD_GATEWAY:
            case GATEWAY_TIMEOUT:
            case SERVICE_UNAVAILABLE:
                return tryFindCachedResponse(resourceReference, cacheHint).orElse(nextLayerResponse);

            default:
                return nextLayerResponse;
        }
    }

    private Optional<Response> tryFindCachedResponse(ResourceReference resourceReference, CacheHint cacheHint) {
        return cache.findCached(resourceReference, cacheHint).map(this::getResponseWithAgeHeader);
    }

    private ResourceReference asResourceReference(ServiceId serviceId, String path) {
        return new ResourceReference(serviceId, path);
    }

    private Response getResponseWithAgeHeader(SerializedResponse serializedResponse) {
        val response = serializedResponse.getResponse();
        val ageSeconds = SECONDS.between(serializedResponse.getLastUpdate(), LocalDateTime.now());
        response.getHttpHeaders().put("Age", singletonList(Long.toString(ageSeconds)));
        return response;
    }
}
