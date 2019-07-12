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

package com.intel.rsd.resourcemanager.layers.merger.request.creators;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.merger.request.ExecutableRequest;
import org.springframework.core.annotation.Order;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Map;

import static com.intel.rsd.collections.IterableHelper.optionalSingle;
import static org.springframework.core.Ordered.HIGHEST_PRECEDENCE;

@Component
@Order(HIGHEST_PRECEDENCE)
class DefaultReadWriteRequestCreator implements SelectableRequestCreator {

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return RW_METHODS.contains(httpMethod);
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        return (Layer layer) -> invoke(path, method, headers, body, requestParams, layer);
    }

    protected Response[] invoke(String path,
                                HttpMethod method,
                                HttpHeaders headers,
                                JsonNode body,
                                Map<String, String> requestParams,
                                Layer layer) {

        return optionalSingle(layer.getServicesExposingPath(path))
            .map(id -> layer.invoke(id, path, method, headers, body, requestParams))
            .map(response -> new Response[]{response}).orElseGet(() -> new Response[0]);
    }
}
