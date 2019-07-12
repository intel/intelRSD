/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.requestenhancer;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Map;
import java.util.Set;

import static org.springframework.http.HttpHeaders.CACHE_CONTROL;

@Component
public class RequestEnhancer extends Layer {

    private final GlobalResources globalResources;

    @Autowired
    public RequestEnhancer(GlobalResources globalResources) {
        this.globalResources = globalResources;
    }

    @Override
    public Set<ServiceId> getServices() {
        return nextLayer.getServices();
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {

        val enhancedHeaders = enhanceHeaders(path, headers);
        return passToNextLayer(serviceId, path, method, enhancedHeaders, body, requestParams);
    }

    private HttpHeaders enhanceHeaders(String path, HttpHeaders headers) {
        //Requests related with global resources should rely on cached responses by default
        if (globalResources.isGlobalResource(path) && !headers.containsKey(CACHE_CONTROL)) {
            val enhancedHeaders = new HttpHeaders();
            enhancedHeaders.putAll(headers);
            enhancedHeaders.setCacheControl("max-stale=666");
            return enhancedHeaders;
        }
        return headers;
    }
}


